#include <okn/script/runtime/runtime_python.hpp>
namespace okn::script {
PyRuntime::PyRuntime() { initialized_ = true; }
PyRuntime::~PyRuntime() = default;
auto PyRuntime::create_context() -> IScriptContext* { return new PyContext(); }
auto PyRuntime::destroy_context(IScriptContext* ctx) -> void { delete ctx; }
auto PyRuntime::version() const -> std::string { return "Python-3.13"; }
PyContext::PyContext() = default;
PyContext::~PyContext() = default;
auto PyContext::load_string(const std::string&, const std::string&) -> bool { return true; }
auto PyContext::load_file(const std::string&) -> bool { return true; }
auto PyContext::call(const std::string&, f32) -> bool { return true; }
} // namespace okn::script
