#pragma once

#include <okn/script/runtime/lua_runtime.hpp>
#include <memory>
#include <vector>
#include <string>

namespace okn::script {

class ScriptEngine {
public:
    ScriptEngine();
    ~ScriptEngine();

    auto create_context() -> LuaContext*;
    void destroy_context(LuaContext* ctx);
    auto default_context() -> LuaContext*;
    auto eval(const std::string& code) -> bool;

private:
    LuaRuntime runtime_;
    std::vector<std::unique_ptr<LuaContext>> contexts_;
    LuaContext* default_ctx_ = nullptr;
};

} // namespace okn::script
