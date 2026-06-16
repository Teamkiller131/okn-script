// Verifies the engine Lua bindings are real and callable (were empty metatables).
#if defined(OKN_SCRIPT_HAS_LUA) && defined(OKN_SCRIPT_HAS_SOL2)

#include <doctest/doctest.h>
#include <okn/script/runtime/lua_runtime.hpp>
#include <okn/script/binding/engine_binding.hpp>

#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif
#include <lua.hpp>
#include <sol/sol.hpp>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

using namespace okn::script;

TEST_CASE("engine bindings: vec3 / world / input / entity callable from Lua") {
    LuaRuntime rt;
    auto* ctx = static_cast<LuaContext*>(rt.create_context());
    REQUIRE(ctx != nullptr);
    EngineBindings::register_all(*ctx);

    auto* L = static_cast<lua_State*>(ctx->get_state());
    REQUIRE(L != nullptr);

    // The sol::state_view holds Lua registry references, so it MUST be destroyed
    // before destroy_context() closes the state (else its dtor unrefs a freed L).
    {
        sol::state_view lua(L);
        auto res = lua.safe_script(R"LUA(
            local a = vec3(1, 2, 2)
            vlen = vec3_length(a)              -- 3
            local b = vec3_add(a, vec3(1, 0, 0))
            sum_x = b.x                        -- 2
            dotv = vec3_dot(a, vec3(1, 0, 0))  -- 1
            okn.log("hello from lua bindings")
            okn_input.set_key("jump", true)
            jump_down = okn_input.key_down("jump")
            miss_down = okn_input.key_down("nope")
            ent_id = entity(42).id
        )LUA", sol::script_pass_on_error);
        REQUIRE(res.valid());

        CHECK(lua["vlen"].get<double>() == doctest::Approx(3.0));
        CHECK(lua["sum_x"].get<double>() == doctest::Approx(2.0));
        CHECK(lua["dotv"].get<double>() == doctest::Approx(1.0));
        CHECK(lua["jump_down"].get<bool>() == true);
        CHECK(lua["miss_down"].get<bool>() == false);
        CHECK(lua["ent_id"].get<int>() == 42);
    }

    rt.destroy_context(ctx);
}

#endif
