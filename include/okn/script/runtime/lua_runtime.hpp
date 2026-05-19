#pragma once

#include <okn/script/script_types.hpp>
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
    auto is_valid() const noexcept -> bool override;
    auto get_state() -> void*; // internal: returns lua_State* cast to void*

    void register_function(const std::string& name, void* fn);
    template <class T> void set_global(const std::string& name, const T& value);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace okn::script
