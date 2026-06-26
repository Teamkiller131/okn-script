// A Lua "game rules" FILE drives the live okn-ecs World each tick, and hot-reloading
// the file (re-running it) swaps the rules while the World persists. The reload here is
// done deterministically (re-execute the changed file) — the same action okn-script's
// HotReload fires from its mtime watch in a real game loop, minus the timing flakiness.
#if defined(OKN_SCRIPT_HAS_LUA) && defined(OKN_SCRIPT_HAS_SOL2)

#include <doctest/doctest.h>
#include <okn/script/runtime/lua_runtime.hpp>
#include <okn/script/binding/ecs_binding.hpp>
#include <okn/ecs/world.hpp>
#include <okn/ecs/scripting/scripting_bridge.hpp>

#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif
#include <lua.hpp>
#include <sol/sol.hpp>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <cstdio>
#include <fstream>

using namespace okn::script;

namespace {
struct Health { int hp = 0; };
}  // namespace

TEST_CASE("okn-script: hot-reloadable Lua rules drive the ECS; the World persists across reload") {
    okn::ecs::World world;
    okn::ecs::ScriptingBridge bridge(world);
    bridge.register_component<Health>("Health");

    LuaRuntime rt;
    auto* ctx = static_cast<LuaContext*>(rt.create_context());
    REQUIRE(ctx != nullptr);
    auto* L = static_cast<lua_State*>(ctx->get_state());
    REQUIRE(L != nullptr);

    {
        sol::state_view lua(L);
        bind_ecs(lua, world, bridge);
        lua.new_usertype<Health>("Health", "hp", &Health::hp);
        lua.set_function("get_health", [&bridge](okn::ecs::Entity e) -> Health* {
            return static_cast<Health*>(bridge.component_data(e, "Health"));
        });

        const char* path = "okn_lua_rules.lua";
        auto write_rules = [&](const char* src) { std::ofstream f(path); f << src; };
        auto reload = [&]() {
            auto r = lua.safe_script_file(path, sol::script_pass_on_error);  // hot-load the file
            REQUIRE(r.valid());
        };

        // v1 rules: each tick, spawn a Health=100 entity until 3 exist.
        write_rules(R"LUA(
            function on_tick()
                if #query("Health") < 3 then
                    local e = create_entity()
                    add_component(e, "Health")
                    get_health(e).hp = 100
                end
            end
        )LUA");
        reload();
        for (int i = 0; i < 6; ++i) { lua["on_tick"](); }
        CHECK(bridge.query("Health").size() == 3);   // v1 cap

        // HOT-RELOAD v2 rules: cap raised to 5, new spawns get hp=50. The World — and the
        // 3 entities already in it — persist across the reload; only the rules change.
        write_rules(R"LUA(
            function on_tick()
                if #query("Health") < 5 then
                    local e = create_entity()
                    add_component(e, "Health")
                    get_health(e).hp = 50
                end
            end
        )LUA");
        reload();
        for (int i = 0; i < 6; ++i) { lua["on_tick"](); }
        CHECK(bridge.query("Health").size() == 5);   // v2 cap (3 kept + 2 new)

        // Persistence proof: the 3 original entities still carry v1's hp=100, the 2 new
        // ones carry v2's hp=50 — the reload swapped rules without resetting the World.
        int n100 = 0, n50 = 0;
        for (const okn::ecs::Entity e : bridge.query("Health")) {
            const Health* h = world.get_component<Health>(e);
            if (h != nullptr && h->hp == 100) { ++n100; }
            if (h != nullptr && h->hp == 50) { ++n50; }
        }
        CHECK(n100 == 3);
        CHECK(n50 == 2);

        std::remove(path);
    }

    rt.destroy_context(ctx);
}

#endif
