#include <okn/script/sandbox/sandbox_policies.hpp>
#include <algorithm>
namespace okn::script {
SandboxPolicy::SandboxPolicy() = default;
SandboxPolicy::~SandboxPolicy() = default;
auto SandboxPolicy::add_allowlist(const std::string& api_name) -> void { allowlist_.push_back(api_name); }
auto SandboxPolicy::add_blocklist(const std::string& api_name) -> void { blocklist_.push_back(api_name); }
auto SandboxPolicy::is_allowed(const std::string& api_name) const -> bool {
    for (const auto& blocked : blocklist_) { if (api_name == blocked) return false; }
    if (allowlist_.empty()) return true;
    for (const auto& allowed : allowlist_) { if (api_name == allowed) return true; }
    return false;
}
auto SandboxPolicy::set_readonly_path(const std::string& path) -> void { readonly_paths_.push_back(path); }
auto SandboxPolicy::set_writeable_path(const std::string& path) -> void { writeable_paths_.push_back(path); }
auto SandboxPolicy::is_path_allowed(const std::string& path, bool write) const -> bool {
    if (write && !fs_enabled_) return false;
    const auto& paths = write ? writeable_paths_ : readonly_paths_;
    for (const auto& allowed : paths) { if (path.find(allowed) == 0) return true; }
    return paths.empty();
}
auto SandboxPolicy::reset() -> void { allowlist_.clear(); blocklist_.clear(); readonly_paths_.clear(); writeable_paths_.clear(); fs_enabled_ = true; network_enabled_ = true; os_enabled_ = true; }
} // namespace okn::script
