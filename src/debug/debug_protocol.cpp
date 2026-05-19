#include <okn/script/debug/debug_protocol.hpp>
namespace okn::script {
DebugProtocol::DebugProtocol() = default;
DebugProtocol::~DebugProtocol() = default;
auto DebugProtocol::encode_request(const std::string& command, const std::string& args) -> std::string { return "{\"command\":\"" + command + "\",\"args\":" + args + "}"; }
auto DebugProtocol::encode_response(u32 seq, const std::string& result) -> std::string { return "{\"seq\":" + std::to_string(seq) + ",\"result\":" + result + "}"; }
auto DebugProtocol::encode_event(const std::string& event, const std::string& data) -> std::string { return "{\"event\":\"" + event + "\",\"data\":" + data + "}"; }
auto DebugProtocol::decode(const std::string& raw) -> DebugMessage { DebugMessage msg; msg.payload = raw; return msg; }
} // namespace okn::script
