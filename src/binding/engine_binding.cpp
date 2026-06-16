#ifdef OKN_SCRIPT_HAS_LUA
#include <okn/script/binding/engine_binding.hpp>
#include <okn/script/runtime/lua_runtime.hpp>

#if defined(OKN_SCRIPT_HAS_SOL2)

#include <cmath>
#include <cstdio>
#include <string>
#include <unordered_map>

#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif
#include <lua.hpp>
#include <sol/sol.hpp>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace okn::script {
namespace {

// Input state the host updates and scripts query.
auto key_state() -> std::unordered_map<std::string, bool>& {
    static std::unordered_map<std::string, bool> m;
    return m;
}

auto vc(const sol::table& t, const char* k) -> double { return t.get_or(k, 0.0); }

} // namespace

// vec3/entity are exposed as plain Lua tables built by factory functions (rather
// than sol usertypes), which is robust across the sol2/Lua build here and matches
// the rest of the engine's scripting layer.
auto EngineBindings::register_vec3(LuaContext& ctx) -> void {
    auto* L = static_cast<lua_State*>(ctx.get_state());
    if (L == nullptr) { return; }
    sol::state_view lua(L);
    lua.set_function("vec3", [](double x, double y, double z, sol::this_state ts) {
        return sol::state_view(ts).create_table_with("x", x, "y", y, "z", z);
    });
    lua.set_function("vec3_length", [](sol::table v) {
        return std::sqrt(vc(v, "x") * vc(v, "x") + vc(v, "y") * vc(v, "y") +
                         vc(v, "z") * vc(v, "z"));
    });
    lua.set_function("vec3_dot", [](sol::table a, sol::table b) {
        return vc(a, "x") * vc(b, "x") + vc(a, "y") * vc(b, "y") + vc(a, "z") * vc(b, "z");
    });
    lua.set_function("vec3_add", [](sol::table a, sol::table b, sol::this_state ts) {
        return sol::state_view(ts).create_table_with(
            "x", vc(a, "x") + vc(b, "x"),
            "y", vc(a, "y") + vc(b, "y"),
            "z", vc(a, "z") + vc(b, "z"));
    });
}

auto EngineBindings::register_entity(LuaContext& ctx) -> void {
    auto* L = static_cast<lua_State*>(ctx.get_state());
    if (L == nullptr) { return; }
    sol::state_view lua(L);
    lua.set_function("entity", [](unsigned id, sol::this_state ts) {
        return sol::state_view(ts).create_table_with("id", id);
    });
}

auto EngineBindings::register_world(LuaContext& ctx) -> void {
    auto* L = static_cast<lua_State*>(ctx.get_state());
    if (L == nullptr) { return; }
    sol::state_view lua(L);
    sol::table okn = lua["okn"].get_or_create<sol::table>();
    okn["version"] = "OmniKillerNexus";
    okn.set_function("log", [](const std::string& msg) {
        std::printf("[lua] %s\n", msg.c_str());
    });
}

auto EngineBindings::register_input(LuaContext& ctx) -> void {
    auto* L = static_cast<lua_State*>(ctx.get_state());
    if (L == nullptr) { return; }
    sol::state_view lua(L);
    sol::table input = lua["okn_input"].get_or_create<sol::table>();
    input.set_function("set_key", [](const std::string& k, bool down) {
        key_state()[k] = down;
    });
    input.set_function("key_down", [](const std::string& k) -> bool {
        auto it = key_state().find(k);
        return it != key_state().end() && it->second;
    });
}

auto EngineBindings::register_all(LuaContext& ctx) -> void {
    if (ctx.get_state() == nullptr) { return; }
    register_vec3(ctx);
    register_entity(ctx);
    register_world(ctx);
    register_input(ctx);
}

} // namespace okn::script

#else  // !OKN_SCRIPT_HAS_SOL2 — sol2 unavailable: keep empty (no-op) bindings.

#include <lua.hpp>
namespace okn::script {
auto EngineBindings::register_all(LuaContext& ctx) -> void { (void)ctx.get_state(); }
auto EngineBindings::register_vec3(LuaContext&) -> void {}
auto EngineBindings::register_entity(LuaContext&) -> void {}
auto EngineBindings::register_world(LuaContext&) -> void {}
auto EngineBindings::register_input(LuaContext&) -> void {}
} // namespace okn::script

#endif  // OKN_SCRIPT_HAS_SOL2
#endif  // OKN_SCRIPT_HAS_LUA
