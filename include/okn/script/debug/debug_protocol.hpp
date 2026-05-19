#pragma once
#include <okn/script/script_types.hpp>
#include <string>
#include <vector>
#include <cstdint>
namespace okn::script {
enum class DebugMessageType : u32 { kRequest = 0, kResponse = 1, kEvent = 2 };
struct DebugMessage {
    DebugMessageType type = DebugMessageType::kRequest;
    u32 seq = 0;
    std::string command;
    std::string payload;
};
class DebugProtocol {
public:
    DebugProtocol();
    ~DebugProtocol();
    auto encode_request(const std::string& command, const std::string& args) -> std::string;
    auto encode_response(u32 seq, const std::string& result) -> std::string;
    auto encode_event(const std::string& event, const std::string& data) -> std::string;
    auto decode(const std::string& raw) -> DebugMessage;
    [[nodiscard]] auto is_connected() const noexcept -> bool { return connected_; }
    auto set_connected(bool v) -> void { connected_ = v; }
private:
    bool connected_ = false;
    u32 next_seq_ = 1;
};
} // namespace okn::script
