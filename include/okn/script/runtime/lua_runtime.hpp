#pragma once

#if defined(OKN_SCRIPT_HAS_LUA)
#include <lua.hpp>
#endif

#include <okn/script/script_types.hpp>
#include <lua.hpp>
#include <string>

namespace okn::script {

class LuaRuntime : public IScriptRuntime {
public:
    LuaRuntime();
    ~LuaRuntime() override;

    auto create_context() -> IScriptContext* override;
    void destroy_context(IScriptContext* ctx) override;
    auto version() const -> std::string override { return "Lua 5.4"; }
};

class LuaContext : public IScriptContext {
public:
    LuaContext();
    ~LuaContext() override;

    auto load_string(const std::string& code, const std::string& name = "inline") -> bool override;
    auto load_file(const std::string& path) -> bool override;
    auto call(const std::string& function, f32 arg = 0.0f) -> bool override;
    auto is_valid() const noexcept -> bool override { return state_ != nullptr; }

    auto get_state() -> lua_State* { return state_; }
    void register_function(const std::string& name, lua_CFunction fn);

    template <class T>
    void set_global(const std::string& name, const T& value);

private:
    lua_State* state_ = nullptr;
};

} // namespace okn::script
