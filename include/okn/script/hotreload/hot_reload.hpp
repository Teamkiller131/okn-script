#pragma once

#include <okn/core/api/types.hpp>
#include <string>
#include <vector>
#include <functional>

namespace okn::script {

class LuaContext;

class HotReload {
public:
    explicit HotReload(LuaContext& ctx);

    auto watch(const std::string& path) -> void;
    auto unwatch(const std::string& path) -> void;
    void update();
    void set_callback(std::function<void(const std::string&)> on_reload);

private:
    LuaContext* ctx_;

    struct WatchedFile {
        std::string path;
        okn::core::i64 last_modified;
    };

    std::vector<WatchedFile> watched_;
    std::function<void(const std::string&)> callback_;
};

} // namespace okn::script
