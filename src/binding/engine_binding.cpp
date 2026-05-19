#ifdef OKN_SCRIPT_HAS_LUA
#include <okn/script/binding/engine_binding.hpp>
#include <okn/script/runtime/lua_runtime.hpp>
#include <lua.hpp>

namespace okn::script {

auto EngineBindings::register_all(LuaContext& ctx) -> void {
    auto* L = static_cast<lua_State*>(ctx.get_state());
    if (!L) return;
    register_vec3(ctx);
    register_entity(ctx);
    register_world(ctx);
    register_input(ctx);
}

auto EngineBindings::register_vec3(LuaContext& ctx) -> void {
    auto* L = static_cast<lua_State*>(ctx.get_state());
    if (!L) return;
    luaL_newmetatable(L, "okn.vec3");
}

auto EngineBindings::register_entity(LuaContext& ctx) -> void {
    auto* L = static_cast<lua_State*>(ctx.get_state());
    if (!L) return;
    luaL_newmetatable(L, "okn.entity");
}

auto EngineBindings::register_world(LuaContext& ctx) -> void {
    auto* L = static_cast<lua_State*>(ctx.get_state());
    if (!L) return;
    lua_createtable(L, 0, 0);
    lua_setglobal(L, "okn_world");
}

auto EngineBindings::register_input(LuaContext& ctx) -> void {
    auto* L = static_cast<lua_State*>(ctx.get_state());
    if (!L) return;
    lua_createtable(L, 0, 0);
    lua_setglobal(L, "okn_input");
}

} // namespace okn::script

#endif
