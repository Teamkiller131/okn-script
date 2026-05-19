#pragma once
#include <okn/script/script_types.hpp>
#include <string>
#include <vector>
#include <functional>
namespace okn::script {
struct Breakpoint { std::string file; u32 line = 0; u32 id = 0; bool enabled = true; };
struct StackFrame { std::string file; u32 line = 0; std::string function_name; };
class DebugServer {
public:
    DebugServer();
    ~DebugServer();
    auto start(u32 port = 9229) -> bool;
    auto stop() -> void;
    auto add_breakpoint(const std::string& file, u32 line) -> u32;
    auto remove_breakpoint(u32 id) -> void;
    auto step_over() -> void;
    auto step_into() -> void;
    auto step_out() -> void;
    auto continue_execution() -> void;
    [[nodiscard]] auto stack_trace() const -> std::vector<StackFrame>;
    [[nodiscard]] auto breakpoints() const -> std::vector<Breakpoint>;
    [[nodiscard]] auto is_running() const noexcept -> bool { return running_; }
    [[nodiscard]] auto is_paused() const noexcept -> bool { return paused_; }
private:
    bool running_ = false;
    bool paused_ = false;
    u32 port_ = 9229;
    std::vector<Breakpoint> breakpoints_;
    u32 next_bp_id_ = 1;
};
} // namespace okn::script
