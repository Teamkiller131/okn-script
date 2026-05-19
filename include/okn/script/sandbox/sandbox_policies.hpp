#pragma once
#include <okn/script/script_types.hpp>
#include <string>
#include <vector>
namespace okn::script {
class SandboxPolicy {
public:
    SandboxPolicy();
    ~SandboxPolicy();
    auto add_allowlist(const std::string& api_name) -> void;
    auto add_blocklist(const std::string& api_name) -> void;
    [[nodiscard]] auto is_allowed(const std::string& api_name) const -> bool;
    auto set_readonly_path(const std::string& path) -> void;
    auto set_writeable_path(const std::string& path) -> void;
    [[nodiscard]] auto is_path_allowed(const std::string& path, bool write) const -> bool;
    auto disable_file_system() -> void { fs_enabled_ = false; }
    auto disable_network() -> void { network_enabled_ = false; }
    auto disable_os_access() -> void { os_enabled_ = false; }
    auto reset() -> void;
private:
    std::vector<std::string> allowlist_;
    std::vector<std::string> blocklist_;
    std::vector<std::string> readonly_paths_;
    std::vector<std::string> writeable_paths_;
    bool fs_enabled_ = true;
    bool network_enabled_ = true;
    bool os_enabled_ = true;
};
} // namespace okn::script
