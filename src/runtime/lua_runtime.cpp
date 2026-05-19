#include <okn/script/runtime/lua_runtime.hpp>

namespace okn::script {

LuaRuntime::LuaRuntime() = default;

LuaRuntime::~LuaRuntime() = default;

auto LuaRuntime::create_context() -> IScriptContext*
{
    return new LuaContext();
}

void LuaRuntime::destroy_context(IScriptContext* ctx)
{
    delete static_cast<LuaContext*>(ctx);
}

LuaContext::LuaContext()
{
    state_ = luaL_newstate();
    if (state_) {
        luaL_openlibs(state_);
    }
}

LuaContext::~LuaContext()
{
    if (state_) {
        lua_close(state_);
        state_ = nullptr;
    }
}

auto LuaContext::load_string(const std::string& code, const std::string& name) -> bool
{
    if (!state_) return false;
    if (luaL_loadbuffer(state_, code.c_str(), code.size(), name.c_str()) != LUA_OK) {
        return false;
    }
    return lua_pcall(state_, 0, 0, 0) == LUA_OK;
}

auto LuaContext::load_file(const std::string& path) -> bool
{
    if (!state_) return false;
    if (luaL_loadfile(state_, path.c_str()) != LUA_OK) {
        return false;
    }
    return lua_pcall(state_, 0, 0, 0) == LUA_OK;
}

auto LuaContext::call(const std::string& function, f32 arg) -> bool
{
    if (!state_) return false;
    lua_getglobal(state_, function.c_str());
    if (!lua_isfunction(state_, -1)) {
        lua_pop(state_, 1);
        return false;
    }
    lua_pushnumber(state_, arg);
    if (lua_pcall(state_, 1, 0, 0) != LUA_OK) {
        return false;
    }
    return true;
}

void LuaContext::register_function(const std::string& name, lua_CFunction fn)
{
    if (!state_) return;
    lua_register(state_, name.c_str(), fn);
}

template <>
void LuaContext::set_global<f32>(const std::string& name, const f32& value)
{
    if (!state_) return;
    lua_pushnumber(state_, value);
    lua_setglobal(state_, name.c_str());
}

template <>
void LuaContext::set_global<core::i32>(const std::string& name, const core::i32& value)
{
    if (!state_) return;
    lua_pushinteger(state_, value);
    lua_setglobal(state_, name.c_str());
}

template <>
void LuaContext::set_global<core::u32>(const std::string& name, const core::u32& value)
{
    if (!state_) return;
    lua_pushinteger(state_, static_cast<lua_Integer>(value));
    lua_setglobal(state_, name.c_str());
}

template <>
void LuaContext::set_global<std::string>(const std::string& name, const std::string& value)
{
    if (!state_) return;
    lua_pushstring(state_, value.c_str());
    lua_setglobal(state_, name.c_str());
}

template <>
void LuaContext::set_global<const char*>(const std::string& name, const char* const& value)
{
    if (!state_) return;
    lua_pushstring(state_, value);
    lua_setglobal(state_, name.c_str());
}

template <>
void LuaContext::set_global<bool>(const std::string& name, const bool& value)
{
    if (!state_) return;
    lua_pushboolean(state_, value ? 1 : 0);
    lua_setglobal(state_, name.c_str());
}

} // namespace okn::script
