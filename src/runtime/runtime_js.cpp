#include <okn/script/runtime/runtime_js.hpp>
namespace okn::script {
JsRuntime::JsRuntime() { initialized_ = true; }
JsRuntime::~JsRuntime() = default;
auto JsRuntime::create_context() -> IScriptContext* { return new JsContext(); }
auto JsRuntime::destroy_context(IScriptContext* ctx) -> void { delete ctx; }
auto JsRuntime::version() const -> std::string { return "QuickJS-2024"; }
JsContext::JsContext() = default;
JsContext::~JsContext() = default;
auto JsContext::load_string(const std::string&, const std::string&) -> bool { return true; }
auto JsContext::load_file(const std::string&) -> bool { return true; }
auto JsContext::call(const std::string&, f32) -> bool { return true; }
} // namespace okn::script
