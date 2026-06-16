#ifdef OKN_SCRIPT_HAS_LUA
#include <okn/script/runtime/lua_runtime.hpp>

#if defined(OKN_SCRIPT_HAS_SOL2)

// sol2-backed Lua runtime (adopted over the raw Lua C API). load_string/load_file
// now actually RUN the chunk (the raw version only loaded it), and set_global is
// real. The state is owned by sol::state, so mixing sol::state_view elsewhere
// (e.g. EngineBindings) is safe.

#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif
#include <lua.hpp>
#include <sol/sol.hpp>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace okn::script {

struct LuaContext::Impl {
    sol::state lua;
};

LuaRuntime::LuaRuntime() = default;
LuaRuntime::~LuaRuntime() = default;
auto LuaRuntime::create_context() -> IScriptContext* { return new LuaContext(); }
void LuaRuntime::destroy_context(IScriptContext* ctx) { delete static_cast<LuaContext*>(ctx); }

LuaContext::LuaContext() : impl_(std::make_unique<Impl>()) {
    impl_->lua.open_libraries();   // all standard libraries
}
LuaContext::~LuaContext() = default;

auto LuaContext::load_string(const std::string& code, const std::string& name) -> bool {
    sol::protected_function_result r =
        impl_->lua.safe_script(code, sol::script_pass_on_error,
                               name.empty() ? "inline" : name);
    return r.valid();
}
auto LuaContext::load_file(const std::string& path) -> bool {
    sol::protected_function_result r =
        impl_->lua.safe_script_file(path, sol::script_pass_on_error);
    return r.valid();
}
auto LuaContext::call(const std::string& function, f32 arg) -> bool {
    sol::protected_function fn = impl_->lua[function];
    if (!fn.valid()) { return false; }
    sol::protected_function_result r = fn(arg);
    return r.valid();
}
auto LuaContext::is_valid() const noexcept -> bool { return impl_ != nullptr; }
auto LuaContext::get_state() -> void* {
    return impl_ ? static_cast<void*>(impl_->lua.lua_state()) : nullptr;
}
void LuaContext::register_function(const std::string& name, void* fn) {
    if (fn == nullptr || !impl_) { return; }
    lua_register(impl_->lua.lua_state(), name.c_str(), reinterpret_cast<lua_CFunction>(fn));
}
template <class T>
void LuaContext::set_global(const std::string& name, const T& value) {
    impl_->lua[name] = value;
}
template void LuaContext::set_global<int>(const std::string&, const int&);
template void LuaContext::set_global<float>(const std::string&, const float&);
template void LuaContext::set_global<double>(const std::string&, const double&);
template void LuaContext::set_global<std::string>(const std::string&, const std::string&);

} // namespace okn::script

#else  // !OKN_SCRIPT_HAS_SOL2 — raw Lua C API fallback (load_string only loads).

#include <lua.hpp>
namespace okn::script {
struct LuaContext::Impl { lua_State* state = nullptr; };
LuaRuntime::LuaRuntime() = default;
LuaRuntime::~LuaRuntime() = default;
auto LuaRuntime::create_context() -> IScriptContext* { return new LuaContext(); }
void LuaRuntime::destroy_context(IScriptContext* ctx) { delete static_cast<LuaContext*>(ctx); }
LuaContext::LuaContext() : impl_(std::make_unique<Impl>()) { impl_->state = luaL_newstate(); if (impl_->state) luaL_openlibs(impl_->state); }
LuaContext::~LuaContext() { if (impl_ && impl_->state) { lua_close(impl_->state); impl_->state = nullptr; } }
auto LuaContext::load_string(const std::string& code, const std::string& name) -> bool { if (!impl_->state) return false; return luaL_loadbuffer(impl_->state, code.c_str(), code.size(), name.c_str()) == LUA_OK; }
auto LuaContext::load_file(const std::string& path) -> bool { if (!impl_->state) return false; return luaL_loadfile(impl_->state, path.c_str()) == LUA_OK; }
auto LuaContext::call(const std::string& function, f32 arg) -> bool { if (!impl_->state) return false; lua_getglobal(impl_->state, function.c_str()); if (!lua_isfunction(impl_->state, -1)) { lua_pop(impl_->state, 1); return false; } lua_pushnumber(impl_->state, arg); return lua_pcall(impl_->state, 1, 0, 0) == LUA_OK; }
auto LuaContext::is_valid() const noexcept -> bool { return impl_ && impl_->state != nullptr; }
auto LuaContext::get_state() -> void* { return static_cast<void*>(impl_ ? impl_->state : nullptr); }
void LuaContext::register_function(const std::string& name, void* fn) { if (!impl_->state) return; lua_register(impl_->state, name.c_str(), reinterpret_cast<lua_CFunction>(fn)); }
template <class T> void LuaContext::set_global(const std::string& name, const T& value) { (void)name; (void)value; }
template void LuaContext::set_global<int>(const std::string&, const int&);
template void LuaContext::set_global<float>(const std::string&, const float&);
template void LuaContext::set_global<double>(const std::string&, const double&);
template void LuaContext::set_global<std::string>(const std::string&, const std::string&);
} // namespace okn::script

#endif  // OKN_SCRIPT_HAS_SOL2
#endif  // OKN_SCRIPT_HAS_LUA
