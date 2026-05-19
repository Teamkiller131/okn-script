#pragma once
#include <okn/script/script_types.hpp>
#include <string>
namespace okn::script {
class LuaRuntime : public IScriptRuntime {
public:
    LuaRuntime();
    ~LuaRuntime() override;
    [[nodiscard]] auto create_context() -> IScriptContext* override;
    auto destroy_context(IScriptContext* ctx) -> void override;
    [[nodiscard]] auto version() const -> std::string override;
private:
    bool initialized_ = false;
};
class LuaContext : public IScriptContext {
public:
    explicit LuaContext();
    ~LuaContext() override;
    auto load_string(const std::string& code, const std::string& name) -> bool override;
    auto load_file(const std::string& path) -> bool override;
    auto call(const std::string& function, f32 arg) -> bool override;
    [[nodiscard]] auto is_valid() const noexcept -> bool override { return valid_; }
private:
    bool valid_ = true;
};
} // namespace okn::script
