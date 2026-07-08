#pragma once

// Bridge okn-ecs's ScriptingBridge to sol2: bind a generic ECS surface into a Lua state
// so a script drives the LIVE World — create/destroy entities, add / has / query
// components by their registered name, and (for components registered WITH FieldDescs)
// read/write scalar fields generically via get_field/set_field. Header-only; include it
// where you already have sol2 + okn-ecs. `world` and `bridge` must outlive the Lua
// state (the bound closures capture them by reference).
//
// For heavy per-component APIs a sol2 usertype over bridge.component_data(e, name) is
// still the richer pattern (see the test); get_field/set_field covers the common case
// with zero per-game binding code.

#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif
#include <sol/sol.hpp>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <okn/ecs/world.hpp>
#include <okn/ecs/scripting/scripting_bridge.hpp>

#include <cstdint>

namespace okn::script {

inline void bind_ecs(sol::state_view lua, okn::ecs::World& world,
                     okn::ecs::ScriptingBridge& bridge) {
    using okn::ecs::Entity;

    // Entities are opaque sol2 usertypes (a script holds handles, not raw ids).
    if (!lua["Entity"].valid()) {
        lua.new_usertype<Entity>("Entity", sol::no_constructor,
                                 "valid", &Entity::is_valid,
                                 "index", &Entity::index);
    }

    lua.set_function("create_entity", [&world]() -> Entity {
        return okn::ecs::ScriptingBridge::script_create_entity(world);
    });
    lua.set_function("destroy_entity", [&world](Entity e) {
        okn::ecs::ScriptingBridge::script_destroy_entity(world, e);
    });
    lua.set_function("has_component", [&bridge](Entity e, const char* name) -> bool {
        return bridge.has_component(e, name);
    });
    lua.set_function("add_component", [&bridge](Entity e, const char* name) {
        bridge.add_component(e, name);
    });
    lua.set_function("query", [&bridge](const char* name) {
        return sol::as_table(bridge.query(name));   // Lua table of Entity handles
    });

    // Generic scalar field access over the bridge's opt-in per-field reflection.
    // Numbers cross the boundary as Lua numbers (doubles); nil / false on a miss.
    using FieldType = okn::ecs::ScriptingBridge::FieldDesc::Type;
    lua.set_function("get_field",
                     [&bridge](Entity e, const char* comp, const char* field)
                         -> sol::optional<double> {
        const auto* fd = bridge.find_field(comp, field);
        void* p = bridge.field_data(e, comp, field);
        if (fd == nullptr || p == nullptr) { return sol::nullopt; }
        switch (fd->type) {
            case FieldType::kF32: return static_cast<double>(*static_cast<float*>(p));
            case FieldType::kF64: return *static_cast<double*>(p);
            case FieldType::kI32: return static_cast<double>(*static_cast<std::int32_t*>(p));
            case FieldType::kU32: return static_cast<double>(*static_cast<std::uint32_t*>(p));
            case FieldType::kU8:  return static_cast<double>(*static_cast<std::uint8_t*>(p));
            case FieldType::kBool: return *static_cast<bool*>(p) ? 1.0 : 0.0;
        }
        return sol::nullopt;
    });
    lua.set_function("set_field",
                     [&bridge](Entity e, const char* comp, const char* field, double v) -> bool {
        const auto* fd = bridge.find_field(comp, field);
        void* p = bridge.field_data(e, comp, field);
        if (fd == nullptr || p == nullptr) { return false; }
        switch (fd->type) {
            case FieldType::kF32: *static_cast<float*>(p) = static_cast<float>(v); break;
            case FieldType::kF64: *static_cast<double*>(p) = v; break;
            case FieldType::kI32: *static_cast<std::int32_t*>(p) = static_cast<std::int32_t>(v); break;
            case FieldType::kU32: *static_cast<std::uint32_t*>(p) = static_cast<std::uint32_t>(v); break;
            case FieldType::kU8:  *static_cast<std::uint8_t*>(p) = static_cast<std::uint8_t>(v); break;
            case FieldType::kBool: *static_cast<bool*>(p) = v != 0.0; break;
        }
        return true;
    });
}

}  // namespace okn::script
