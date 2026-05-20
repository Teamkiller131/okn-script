#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#ifdef OKN_SCRIPT_HAS_LUA
#include <lua.hpp>
#include <lualib.h>
#endif

#include <okn/script/script_engine.hpp>
#include <okn/script/binding/engine_binding.hpp>
#include <okn/script/hotreload/hot_reload.hpp>

namespace okn::script {
namespace {

#define LUA_STATE(ctx) reinterpret_cast<lua_State*>(ctx->get_state())

#ifdef OKN_SCRIPT_HAS_LUA

TEST_CASE("okn::script::ScriptEngine - create and default context") {
    ScriptEngine engine;
    auto* ctx = engine.default_context();
    CHECK(ctx != nullptr);
    CHECK(ctx->is_valid());
}

TEST_CASE("okn::script::ScriptEngine - eval lua code" * doctest::skip()) {
    ScriptEngine engine;
    bool ok = engine.eval("x = 42");
    CHECK(ok);
    auto* ctx = engine.default_context();
    CHECK(ctx != nullptr);
    lua_State* L = LUA_STATE(ctx);
    lua_getglobal(L, "x");
    CHECK(lua_isnumber(L, -1));
    CHECK(lua_tonumber(L, -1) == doctest::Approx(42.0));
    lua_pop(L, 1);
}

TEST_CASE("okn::script::LuaContext - load and call function" * doctest::skip()) {
    LuaRuntime runtime;
    auto* raw_ctx = runtime.create_context();
    REQUIRE(raw_ctx != nullptr);
    auto* ctx = static_cast<LuaContext*>(raw_ctx);
    REQUIRE(ctx->is_valid());
    bool ok = ctx->load_string("function add_one(n) return n + 1 end");
    CHECK(ok);
    lua_State* L = LUA_STATE(ctx);
    lua_getglobal(L, "add_one"); lua_pushnumber(L, 5.0);
    CHECK(lua_pcall(L, 1, 1, 0) == LUA_OK);
    CHECK(lua_tonumber(L, -1) == doctest::Approx(6.0));
    lua_pop(L, 1);
    runtime.destroy_context(ctx);
}

TEST_CASE("okn::script::LuaContext - error handling") {
    LuaRuntime runtime;
    auto* raw_ctx = runtime.create_context();
    REQUIRE(raw_ctx != nullptr);
    auto* ctx = static_cast<LuaContext*>(raw_ctx);
    CHECK(!ctx->load_string("this is not valid lua @@@"));
    CHECK(!ctx->call("nonexistent_function", 0.0f));
    runtime.destroy_context(ctx);
}

TEST_CASE("okn::script::ScriptEngine - multiple contexts") {
    ScriptEngine engine;
    auto* ctx1 = engine.create_context();
    auto* ctx2 = engine.create_context();
    CHECK(ctx1 != nullptr); CHECK(ctx2 != nullptr);
    CHECK(ctx1 != ctx2);
    engine.eval("shared_val = 777");
    engine.destroy_context(ctx2);
    engine.destroy_context(ctx1);
}

#endif // OKN_SCRIPT_HAS_LUA

TEST_CASE("okn::script::ScriptEngine - basic lifecycle (no lua)") {
    ScriptEngine engine;
    CHECK(engine.default_context() != nullptr);
}

} // anonymous namespace
} // namespace okn::script
