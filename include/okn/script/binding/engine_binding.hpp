#pragma once

namespace okn::script {

class LuaContext;

class EngineBindings {
public:
    static auto register_all(LuaContext& ctx) -> void;
    static auto register_vec3(LuaContext& ctx) -> void;
    static auto register_entity(LuaContext& ctx) -> void;
    static auto register_world(LuaContext& ctx) -> void;
    static auto register_input(LuaContext& ctx) -> void;
};

} // namespace okn::script
