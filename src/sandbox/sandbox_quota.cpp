#include <okn/script/sandbox/sandbox_quota.hpp>
namespace okn::script {
SandboxManager::SandboxManager(const SandboxQuota& quota) : quota_(quota) {}
SandboxManager::~SandboxManager() = default;
auto SandboxManager::check_timeout() -> bool { return elapsed_ms_ < quota_.max_time_ms; }
auto SandboxManager::check_memory(std::size_t allocated) -> bool { memory_used_ += allocated; return memory_used_ <= quota_.max_memory_bytes; }
auto SandboxManager::check_instructions(u32 count) -> bool { instructions_executed_ += count; return instructions_executed_ <= quota_.max_instruction_count; }
auto SandboxManager::reset() -> void { elapsed_ms_ = 0; memory_used_ = 0; instructions_executed_ = 0; }
} // namespace okn::script
