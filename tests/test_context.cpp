#include <doctest/doctest.h>
#include <okn/script/script_engine.hpp>
#include <okn/script/runtime/lua_runtime.hpp>
#include <okn/core/api/types.hpp>

using namespace okn::script;

TEST_CASE("okn::script::ScriptEngine - construction") {
    ScriptEngine engine;
    auto* ctx = engine.default_context();
    CHECK(ctx != nullptr);
}

TEST_CASE("okn::script::ScriptEngine - create context") {
    ScriptEngine engine;
    auto* ctx1 = engine.create_context();
    auto* ctx2 = engine.create_context();
    CHECK(ctx1 != nullptr);
    CHECK(ctx2 != nullptr);
    CHECK(ctx1 != ctx2);
    engine.destroy_context(ctx2);
    engine.destroy_context(ctx1);
}

TEST_CASE("okn::script::LuaRuntime - construction") {
    LuaRuntime runtime;
    CHECK(runtime.version() == "Lua 5.4");
}

TEST_CASE("okn::script::LuaRuntime - context lifecycle") {
    LuaRuntime runtime;
    auto* ctx = runtime.create_context();
    CHECK(ctx != nullptr);
    runtime.destroy_context(ctx);
}

TEST_CASE("okn::script::IScriptContext - invalid code") {
    ScriptEngine engine;
    auto* ctx = engine.default_context();
    CHECK(ctx != nullptr);

    // Loading invalid code should fail, not crash
    auto ok = ctx->load_string("@@@@ not valid @@@@");
    CHECK(!ok);

    // Calling non-existent function should fail
    ok = ctx->call("nonexistent_func", 0.0f);
    CHECK(!ok);
}

TEST_CASE("okn::script::IScriptContext - empty strings") {
    ScriptEngine engine;
    auto* ctx = engine.default_context();
    auto ok = ctx->load_string("");
    // Empty string may or may not be valid depending on implementation
    (void)ok;
    CHECK(ctx->is_valid());
}

TEST_CASE("okn::script::ScriptEngine - multiple evals") {
    ScriptEngine engine;
    auto ok1 = engine.eval("x = 10");
    auto ok2 = engine.eval("y = 20");
    auto ok3 = engine.eval("z = x + y");
    (void)ok1; (void)ok2; (void)ok3;
    CHECK(engine.default_context()->is_valid());
}
