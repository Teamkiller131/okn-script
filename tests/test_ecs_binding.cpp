// A Lua script driving the LIVE okn-ecs World through the ScriptingBridge (sol2).
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

using namespace okn::script;

namespace {
struct Pos { double x = 0.0; double y = 0.0; };
struct Vel { double dx = 0.0; double dy = 0.0; };
}  // namespace

TEST_CASE("okn-script: a Lua script drives the live okn-ecs World via the ScriptingBridge") {
    okn::ecs::World world;
    okn::ecs::ScriptingBridge bridge(world);
    bridge.register_component<Pos>("Pos");
    bridge.register_component<Vel>("Vel");

    LuaRuntime rt;
    auto* ctx = static_cast<LuaContext*>(rt.create_context());
    REQUIRE(ctx != nullptr);
    auto* L = static_cast<lua_State*>(ctx->get_state());
    REQUIRE(L != nullptr);

    // The sol::state_view holds Lua registry refs, so it must die before the context
    // closes L. world/bridge outlive both (the bound closures capture them).
    {
        sol::state_view lua(L);
        bind_ecs(lua, world, bridge);

        // Game-specific component field access: a usertype + a getter over the live
        // store's bytes. Writes through `p` hit the same component C++ sees.
        lua.new_usertype<Pos>("Pos", "x", &Pos::x, "y", &Pos::y);
        lua.set_function("get_pos", [&bridge](okn::ecs::Entity e) -> Pos* {
            return static_cast<Pos*>(bridge.component_data(e, "Pos"));
        });

        auto res = lua.safe_script(R"LUA(
            local e1 = create_entity()
            local e2 = create_entity()
            add_component(e1, "Pos")
            add_component(e2, "Pos")
            add_component(e2, "Vel")

            n_pos = #query("Pos")          -- 2
            n_vel = #query("Vel")          -- 1

            local p = get_pos(e1)          -- read/write a component field from Lua
            p.x = 11.0
            p.y = 22.0
            readback_x = get_pos(e1).x     -- re-read through the live store -> 11
            has_e1 = has_component(e1, "Pos")

            destroy_entity(e1)
            n_pos_after = #query("Pos")    -- 1
        )LUA", sol::script_pass_on_error);
        REQUIRE(res.valid());

        CHECK(lua["n_pos"].get<int>() == 2);
        CHECK(lua["n_vel"].get<int>() == 1);
        CHECK(lua["readback_x"].get<double>() == doctest::Approx(11.0));
        CHECK(lua["has_e1"].get<bool>() == true);
        CHECK(lua["n_pos_after"].get<int>() == 1);
    }

    // C++ sees the SAME live World the Lua script mutated: e1 gone, e2 still has Pos.
    CHECK(bridge.query("Pos").size() == 1);
    CHECK(world.entity_count() == 1);

    rt.destroy_context(ctx);
}

TEST_CASE("okn-script: generic get_field/set_field via the bridge's per-field reflection") {
    // The per-field descriptor layer: register Pos WITH FieldDescs and Lua reads/writes
    // its scalars by NAME — no usertype, no per-game binding code.
    using FieldDesc = okn::ecs::ScriptingBridge::FieldDesc;
    okn::ecs::World world;
    okn::ecs::ScriptingBridge bridge(world);
    bridge.register_component<Pos>(
        "Pos", {FieldDesc{"x", offsetof(Pos, x), FieldDesc::Type::kF64},
                FieldDesc{"y", offsetof(Pos, y), FieldDesc::Type::kF64}});
    bridge.register_component<Vel>("Vel");   // opaque: no field descriptors

    LuaRuntime rt;
    auto* ctx = static_cast<LuaContext*>(rt.create_context());
    REQUIRE(ctx != nullptr);
    auto* L = static_cast<lua_State*>(ctx->get_state());
    REQUIRE(L != nullptr);

    const okn::ecs::Entity e = world.create_entity();
    world.add_component(e, Pos{3.0, 4.0});

    {
        sol::state_view lua(L);
        bind_ecs(lua, world, bridge);
        lua["e"] = e;

        auto res = lua.safe_script(R"LUA(
            x0 = get_field(e, "Pos", "x")            -- 3
            ok = set_field(e, "Pos", "y", 42.5)      -- true
            y1 = get_field(e, "Pos", "y")            -- 42.5
            miss = get_field(e, "Pos", "nope")       -- nil (unknown field)
            opaque = get_field(e, "Vel", "dx")       -- nil (no descriptors registered)
        )LUA", sol::script_pass_on_error);
        REQUIRE(res.valid());

        CHECK(lua["x0"].get<double>() == doctest::Approx(3.0));
        CHECK(lua["ok"].get<bool>() == true);
        CHECK(lua["y1"].get<double>() == doctest::Approx(42.5));
        CHECK(lua["miss"].valid() == false);
        CHECK(lua["opaque"].valid() == false);
    }

    // The write landed in the LIVE store C++ sees.
    const Pos* p = world.get_component<Pos>(e);
    REQUIRE(p != nullptr);
    CHECK(p->y == doctest::Approx(42.5));

    rt.destroy_context(ctx);
}

#endif
