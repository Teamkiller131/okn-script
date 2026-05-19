#include <okn/script/hotreload/hot_reload.hpp>
#include <okn/script/runtime/lua_runtime.hpp>
#include <okn/platform/fs/fs_info.hpp>
#include <algorithm>

namespace okn::script {

HotReload::HotReload(LuaContext& ctx)
    : ctx_(&ctx)
{
}

auto HotReload::watch(const std::string& path) -> void
{
    auto attrs = okn::platform::get_file_attributes(path.c_str());
    watched_.push_back({path, attrs.modification_time});
}

auto HotReload::unwatch(const std::string& path) -> void
{
    watched_.erase(
        std::remove_if(watched_.begin(), watched_.end(),
            [&path](const WatchedFile& wf) { return wf.path == path; }),
        watched_.end());
}

void HotReload::update()
{
    if (!ctx_ || !ctx_->is_valid()) return;

    for (auto& wf : watched_) {
        auto attrs = okn::platform::get_file_attributes(wf.path.c_str());
        if (attrs.exists && attrs.modification_time > wf.last_modified) {
            wf.last_modified = attrs.modification_time;

            if (ctx_->load_file(wf.path)) {
                if (callback_) {
                    callback_(wf.path);
                }
            }
        }
    }
}

void HotReload::set_callback(std::function<void(const std::string&)> on_reload)
{
    callback_ = std::move(on_reload);
}

} // namespace okn::script
