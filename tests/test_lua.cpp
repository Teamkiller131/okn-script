#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <okn/script/script_engine.hpp>
#include <okn/script/binding/engine_binding.hpp>
#include <okn/script/hotreload/hot_reload.hpp>

namespace okn::script {
namespace {

TEST_CASE("okn::script::ScriptEngine - create and default context")
{
    ScriptEngine engine;
    auto* ctx = engine.default_context();
    CHECK(ctx != nullptr);
    CHECK(ctx->is_valid());
}

TEST_CASE("okn::script::ScriptEngine - eval lua code")
{
    ScriptEngine engine;
    bool ok = engine.eval("x = 42");
    CHECK(ok);

    auto* ctx = engine.default_context();
    CHECK(ctx != nullptr);

    lua_State* L = ctx->get_state();
    lua_getglobal(L, "x");
    CHECK(lua_isnumber(L, -1));
    CHECK(lua_tonumber(L, -1) == doctest::Approx(42.0));
    lua_pop(L, 1);
}

TEST_CASE("okn::script::LuaContext - load and call function")
{
    LuaRuntime runtime;
    auto* raw_ctx = runtime.create_context();
    REQUIRE(raw_ctx != nullptr);
    auto* ctx = static_cast<LuaContext*>(raw_ctx);
    REQUIRE(ctx->is_valid());

    bool ok = ctx->load_string(R"(
        function greet(name)
            return "Hello, " .. name
        end
    )");
    CHECK(ok);

    ok = ctx->load_string(R"(
        function add_one(n)
            return n + 1
        end
    )");
    CHECK(ok);

    lua_State* L = ctx->get_state();

    lua_getglobal(L, "add_one");
    lua_pushnumber(L, 5.0);
    CHECK(lua_pcall(L, 1, 1, 0) == LUA_OK);
    CHECK(lua_tonumber(L, -1) == doctest::Approx(6.0));
    lua_pop(L, 1);

    lua_getglobal(L, "greet");
    lua_pushstring(L, "World");
    CHECK(lua_pcall(L, 1, 1, 0) == LUA_OK);
    CHECK(std::string(lua_tostring(L, -1)) == "Hello, World");
    lua_pop(L, 1);

    runtime.destroy_context(ctx);
}

TEST_CASE("okn::script::LuaContext - load file")
{
    LuaRuntime runtime;
    auto* raw_ctx = runtime.create_context();
    REQUIRE(raw_ctx != nullptr);
    auto* ctx = static_cast<LuaContext*>(raw_ctx);

    bool ok = ctx->load_string("f = function() return 99 end");
    CHECK(ok);

    bool load_ok = ctx->load_file("nonexistent_file.lua");
    CHECK(!load_ok);

    runtime.destroy_context(ctx);
}

TEST_CASE("okn::script::LuaContext - set_global")
{
    LuaRuntime runtime;
    auto* raw_ctx = runtime.create_context();
    REQUIRE(raw_ctx != nullptr);
    auto* ctx = static_cast<LuaContext*>(raw_ctx);

    ctx->set_global<f32>("pi", 3.14f);
    ctx->set_global<core::i32>("answer", 42);
    ctx->set_global<std::string>("name", std::string("OmniKiller"));
    ctx->set_global<bool>("flag", true);

    lua_State* L = ctx->get_state();

    lua_getglobal(L, "pi");
    CHECK(lua_tonumber(L, -1) == doctest::Approx(3.14));
    lua_pop(L, 1);

    lua_getglobal(L, "answer");
    CHECK(lua_tointeger(L, -1) == 42);
    lua_pop(L, 1);

    lua_getglobal(L, "name");
    CHECK(std::string(lua_tostring(L, -1)) == "OmniKiller");
    lua_pop(L, 1);

    lua_getglobal(L, "flag");
    CHECK(lua_toboolean(L, -1) == 1);
    lua_pop(L, 1);

    runtime.destroy_context(ctx);
}

TEST_CASE("okn::script::LuaContext - register_function")
{
    LuaRuntime runtime;
    auto* raw_ctx = runtime.create_context();
    REQUIRE(raw_ctx != nullptr);
    auto* ctx = static_cast<LuaContext*>(raw_ctx);

    ctx->register_function("double_it", [](lua_State* L) -> int {
        f32 val = static_cast<f32>(lua_tonumber(L, 1));
        lua_pushnumber(L, val * 2.0);
        return 1;
    });

    bool ok = ctx->load_string("result = double_it(21)");
    CHECK(ok);

    lua_State* L = ctx->get_state();
    lua_getglobal(L, "result");
    CHECK(lua_tonumber(L, -1) == doctest::Approx(42.0));
    lua_pop(L, 1);

    runtime.destroy_context(ctx);
}

TEST_CASE("okn::script::EngineBindings - vec3")
{
    LuaRuntime runtime;
    auto* raw_ctx = runtime.create_context();
    REQUIRE(raw_ctx != nullptr);
    auto* ctx = static_cast<LuaContext*>(raw_ctx);

    EngineBindings::register_vec3(*ctx);

    bool ok = ctx->load_string(R"(
        a = vec3.new(1, 2, 3)
        b = vec3.new(4, 5, 6)
        c = a:add(b)
        d = a:dot(b)
    )");
    CHECK(ok);

    lua_State* L = ctx->get_state();

    lua_getglobal(L, "c");
    CHECK(lua_isuserdata(L, -1));
    lua_getfield(L, -1, "x");
    CHECK(lua_tonumber(L, -1) == doctest::Approx(5.0));
    lua_pop(L, 1);
    lua_getfield(L, -1, "y");
    CHECK(lua_tonumber(L, -1) == doctest::Approx(7.0));
    lua_pop(L, 1);
    lua_getfield(L, -1, "z");
    CHECK(lua_tonumber(L, -1) == doctest::Approx(9.0));
    lua_pop(L, 2);

    lua_getglobal(L, "d");
    CHECK(lua_tonumber(L, -1) == doctest::Approx(32.0));
    lua_pop(L, 1);

    runtime.destroy_context(ctx);
}

TEST_CASE("okn::script::EngineBindings - entity")
{
    LuaRuntime runtime;
    auto* raw_ctx = runtime.create_context();
    REQUIRE(raw_ctx != nullptr);
    auto* ctx = static_cast<LuaContext*>(raw_ctx);

    EngineBindings::register_entity(*ctx);

    bool ok = ctx->load_string(R"(
        e = entity.new(100)
    )");
    CHECK(ok);

    runtime.destroy_context(ctx);
}

TEST_CASE("okn::script::EngineBindings - register_all")
{
    LuaRuntime runtime;
    auto* raw_ctx = runtime.create_context();
    REQUIRE(raw_ctx != nullptr);
    auto* ctx = static_cast<LuaContext*>(raw_ctx);

    EngineBindings::register_all(*ctx);

    bool ok = ctx->load_string(R"(
        v = vec3.new(10, 20, 30)
        e = entity.new(1)
    )");
    CHECK(ok);

    runtime.destroy_context(ctx);
}

TEST_CASE("okn::script::HotReload - basic watch and update")
{
    LuaRuntime runtime;
    auto* raw_ctx = runtime.create_context();
    REQUIRE(raw_ctx != nullptr);
    auto* ctx = static_cast<LuaContext*>(raw_ctx);
    
    ctx->load_string("function hot_func() return 'old' end");
    
    HotReload reload(*ctx);
    int reload_count = 0;
    reload.set_callback([&reload_count](const std::string&) { ++reload_count; });
    
    reload.watch("nonexistent_script.lua");
    reload.update();
    
    CHECK(reload_count == 0);
    
    runtime.destroy_context(ctx);
}

TEST_CASE("okn::script::ScriptEngine - multiple contexts")
{
    ScriptEngine engine;
    
    auto* ctx1 = engine.create_context();
    auto* ctx2 = engine.create_context();
    
    CHECK(ctx1 != nullptr);
    CHECK(ctx2 != nullptr);
    CHECK(ctx1 != ctx2);
    CHECK(ctx1->is_valid());
    CHECK(ctx2->is_valid());
    
    auto* def_ctx = engine.default_context();
    CHECK(def_ctx != nullptr);
    CHECK(def_ctx != ctx1);
    CHECK(def_ctx != ctx2);
    
    engine.eval("shared_val = 777");
    
    lua_State* Ld = def_ctx->get_state();
    lua_getglobal(Ld, "shared_val");
    CHECK(lua_tonumber(Ld, -1) == doctest::Approx(777.0));
    lua_pop(Ld, 1);
    
    lua_State* L2 = ctx2->get_state();
    lua_getglobal(L2, "shared_val");
    CHECK(lua_isnil(L2, -1));
    lua_pop(L2, 1);
    
    engine.destroy_context(ctx2);
    engine.destroy_context(ctx1);
}

TEST_CASE("okn::script::LuaContext - error handling")
{
    LuaRuntime runtime;
    auto* raw_ctx = runtime.create_context();
    REQUIRE(raw_ctx != nullptr);
    auto* ctx = static_cast<LuaContext*>(raw_ctx);
    
    bool ok = ctx->load_string("this is not valid lua @@@");
    CHECK(!ok);
    
    ok = ctx->call("nonexistent_function", 0.0f);
    CHECK(!ok);
    
    runtime.destroy_context(ctx);
}

} // anonymous namespace
} // namespace okn::script
