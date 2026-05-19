#pragma once

#include <okn/core/api/types.hpp>
#include <string>
#include <functional>

namespace okn::script {

using okn::core::u32;
using okn::core::f32;

// ── 脚本上下文接口 ──

class IScriptContext {
public:
    virtual ~IScriptContext() = default;
    virtual auto load_string(const std::string& code, const std::string& name = "inline") -> bool = 0;
    virtual auto load_file(const std::string& path) -> bool = 0;
    virtual auto call(const std::string& function, f32 arg = 0.0f) -> bool = 0;
    virtual auto is_valid() const noexcept -> bool = 0;
};

// ── 脚本运行时接口 ──

class IScriptRuntime {
public:
    virtual ~IScriptRuntime() = default;
    virtual auto create_context() -> IScriptContext* = 0;
    virtual void destroy_context(IScriptContext* ctx) = 0;
    virtual auto version() const -> std::string = 0;
};

} // namespace okn::script
