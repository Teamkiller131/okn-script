#include <okn/script/debug/debug_server.hpp>
#include <algorithm>
namespace okn::script {
DebugServer::DebugServer() = default;
DebugServer::~DebugServer() { stop(); }
auto DebugServer::start(u32 port) -> bool { port_ = port; running_ = true; return true; }
auto DebugServer::stop() -> void { running_ = false; paused_ = false; }
auto DebugServer::add_breakpoint(const std::string& file, u32 line) -> u32 {
    Breakpoint bp{file, line, next_bp_id_++, true};
    breakpoints_.push_back(bp);
    return bp.id;
}
auto DebugServer::remove_breakpoint(u32 id) -> void {
    breakpoints_.erase(std::remove_if(breakpoints_.begin(), breakpoints_.end(), [id](const Breakpoint& bp) { return bp.id == id; }), breakpoints_.end());
}
auto DebugServer::step_over() -> void {}
auto DebugServer::step_into() -> void {}
auto DebugServer::step_out() -> void {}
auto DebugServer::continue_execution() -> void { paused_ = false; }
auto DebugServer::stack_trace() const -> std::vector<StackFrame> { return {}; }
auto DebugServer::breakpoints() const -> std::vector<Breakpoint> { return breakpoints_; }
} // namespace okn::script
