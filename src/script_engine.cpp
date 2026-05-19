#include <okn/script/script_engine.hpp>
#include <algorithm>

namespace okn::script {

ScriptEngine::ScriptEngine()
{
    auto* ctx = static_cast<LuaContext*>(runtime_.create_context());
    if (ctx) {
        default_ctx_ = ctx;
        contexts_.push_back(std::unique_ptr<LuaContext>(ctx));
    }
}

ScriptEngine::~ScriptEngine() = default;

auto ScriptEngine::create_context() -> LuaContext*
{
    auto* ctx = static_cast<LuaContext*>(runtime_.create_context());
    if (ctx) {
        contexts_.push_back(std::unique_ptr<LuaContext>(ctx));
    }
    return ctx;
}

void ScriptEngine::destroy_context(LuaContext* ctx)
{
    if (!ctx) return;

    if (ctx == default_ctx_) {
        default_ctx_ = nullptr;
    }

    auto it = std::find_if(contexts_.begin(), contexts_.end(),
        [ctx](const std::unique_ptr<LuaContext>& p) { return p.get() == ctx; });

    if (it != contexts_.end()) {
        runtime_.destroy_context(it->release());
        contexts_.erase(it);
    }
}

auto ScriptEngine::default_context() -> LuaContext*
{
    return default_ctx_;
}

auto ScriptEngine::eval(const std::string& code) -> bool
{
    if (!default_ctx_) return false;
    return default_ctx_->load_string(code);
}

} // namespace okn::script
