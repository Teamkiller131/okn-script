#ifdef OKN_SCRIPT_HAS_LUA
#include <okn/script/runtime/lua_runtime.hpp>

#include <lua.hpp>

namespace okn::script {

struct LuaContext::Impl {
    lua_State* state = nullptr;
};

LuaRuntime::LuaRuntime() = default;
LuaRuntime::~LuaRuntime() = default;

auto LuaRuntime::create_context() -> IScriptContext* {
    return new LuaContext();
}

void LuaRuntime::destroy_context(IScriptContext* ctx) {
    delete static_cast<LuaContext*>(ctx);
}

LuaContext::LuaContext() : impl_(std::make_unique<Impl>()) {
    impl_->state = luaL_newstate();
    if (impl_->state) luaL_openlibs(impl_->state);
}

LuaContext::~LuaContext() {
    if (impl_ && impl_->state) {
        lua_close(impl_->state);
        impl_->state = nullptr;
    }
}

auto LuaContext::load_string(const std::string& code, const std::string& name) -> bool {
    if (!impl_->state) return false;
    return luaL_loadbuffer(impl_->state, code.c_str(), code.size(), name.c_str()) == LUA_OK;
}

auto LuaContext::load_file(const std::string& path) -> bool {
    if (!impl_->state) return false;
    return luaL_loadfile(impl_->state, path.c_str()) == LUA_OK;
}

auto LuaContext::call(const std::string& function, f32 arg) -> bool {
    if (!impl_->state) return false;
    lua_getglobal(impl_->state, function.c_str());
    if (!lua_isfunction(impl_->state, -1)) { lua_pop(impl_->state, 1); return false; }
    lua_pushnumber(impl_->state, arg);
    return lua_pcall(impl_->state, 1, 0, 0) == LUA_OK;
}

auto LuaContext::is_valid() const noexcept -> bool {
    return impl_ && impl_->state != nullptr;
}

auto LuaContext::get_state() -> void* {
    return static_cast<void*>(impl_ ? impl_->state : nullptr);
}

void LuaContext::register_function(const std::string& name, void* fn) {
    if (!impl_->state) return;
    lua_register(impl_->state, name.c_str(), reinterpret_cast<lua_CFunction>(fn));
}

template <class T>
void LuaContext::set_global(const std::string& name, const T& value) {
    (void)name; (void)value;
}

template void LuaContext::set_global<int>(const std::string&, const int&);
template void LuaContext::set_global<float>(const std::string&, const float&);
template void LuaContext::set_global<double>(const std::string&, const double&);
template void LuaContext::set_global<std::string>(const std::string&, const std::string&);

} // namespace okn::script

#endif