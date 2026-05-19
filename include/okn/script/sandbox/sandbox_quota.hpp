#pragma once
#include <okn/script/script_types.hpp>
#include <cstddef>
namespace okn::script {
struct SandboxQuota {
    u32 max_time_ms = 5000;
    std::size_t max_memory_bytes = 64 * 1024 * 1024;
    u32 max_instruction_count = 1000000;
    bool enable_timeout = true;
    bool enable_memory_limit = true;
};
class SandboxManager {
public:
    explicit SandboxManager(const SandboxQuota& quota = {});
    ~SandboxManager();
    auto check_timeout() -> bool;
    auto check_memory(std::size_t allocated) -> bool;
    auto check_instructions(u32 count) -> bool;
    auto reset() -> void;
    [[nodiscard]] auto quota() const -> const SandboxQuota& { return quota_; }
    [[nodiscard]] auto elapsed_ms() const -> u32 { return elapsed_ms_; }
    [[nodiscard]] auto memory_used() const -> std::size_t { return memory_used_; }
    auto set_quota(const SandboxQuota& q) -> void { quota_ = q; }
private:
    SandboxQuota quota_;
    u32 elapsed_ms_ = 0;
    std::size_t memory_used_ = 0;
    u32 instructions_executed_ = 0;
};
} // namespace okn::script
