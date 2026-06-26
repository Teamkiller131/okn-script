#pragma once

// Bridge okn-ecs's ScriptingBridge to sol2: bind a generic ECS surface into a Lua state
// so a script drives the LIVE World — create/destroy entities, and add / has / query
// components by their registered name. Header-only; include it where you already have
// sol2 + okn-ecs. `world` and `bridge` must outlive the Lua state (the bound closures
// capture them by reference).
//
// Component FIELD read/write is intentionally game-specific: register each component as
// a sol2 usertype and a getter over bridge.component_data(e, name) — see the test for
// the pattern. (A generic by-name field surface would need per-field reflection the ECS
// deliberately doesn't carry.)

#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif
#include <sol/sol.hpp>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <okn/ecs/world.hpp>
#include <okn/ecs/scripting/scripting_bridge.hpp>

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
}

}  // namespace okn::script
