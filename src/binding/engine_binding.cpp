#ifdef OKN_SCRIPT_HAS_LUA
#include <okn/script/binding/engine_binding.hpp>
#include <okn/script/runtime/lua_runtime.hpp>
#include <lua.hpp>
#include <lualib.h>
#include <okn/core/api/types.hpp>
#include <string>
#include <cstring>

namespace okn::script {

namespace {

using core::f32;
using core::i32;
using core::u32;
using core::u64;

struct Vec3Userdata {
    f32 x = 0.0f;
    f32 y = 0.0f;
    f32 z = 0.0f;
};

struct EntityUserdata {
    u64 id = 0;
};

void* g_world_ptr = nullptr;
void* g_input_ptr = nullptr;

// ── Vec3 metamethods ──

static auto l_check_vec3(lua_State* L, int idx) -> Vec3Userdata*
{
    return static_cast<Vec3Userdata*>(luaL_checkudata(L, idx, "okn.vec3"));
}

static auto l_vec3_new(lua_State* L) -> int
{
    f32 x = static_cast<f32>(luaL_optnumber(L, 1, 0.0));
    f32 y = static_cast<f32>(luaL_optnumber(L, 2, 0.0));
    f32 z = static_cast<f32>(luaL_optnumber(L, 3, 0.0));
    auto* v = static_cast<Vec3Userdata*>(lua_newuserdata(L, sizeof(Vec3Userdata)));
    v->x = x;
    v->y = y;
    v->z = z;
    luaL_setmetatable(L, "okn.vec3");
    return 1;
}

static auto l_vec3_add(lua_State* L) -> int
{
    auto* a = l_check_vec3(L, 1);
    auto* b = l_check_vec3(L, 2);
    auto* r = static_cast<Vec3Userdata*>(lua_newuserdata(L, sizeof(Vec3Userdata)));
    r->x = a->x + b->x;
    r->y = a->y + b->y;
    r->z = a->z + b->z;
    luaL_setmetatable(L, "okn.vec3");
    return 1;
}

static auto l_vec3_sub(lua_State* L) -> int
{
    auto* a = l_check_vec3(L, 1);
    auto* b = l_check_vec3(L, 2);
    auto* r = static_cast<Vec3Userdata*>(lua_newuserdata(L, sizeof(Vec3Userdata)));
    r->x = a->x - b->x;
    r->y = a->y - b->y;
    r->z = a->z - b->z;
    luaL_setmetatable(L, "okn.vec3");
    return 1;
}

static auto l_vec3_mul(lua_State* L) -> int
{
    auto* v = l_check_vec3(L, 1);
    f32 s = static_cast<f32>(luaL_checknumber(L, 2));
    auto* r = static_cast<Vec3Userdata*>(lua_newuserdata(L, sizeof(Vec3Userdata)));
    r->x = v->x * s;
    r->y = v->y * s;
    r->z = v->z * s;
    luaL_setmetatable(L, "okn.vec3");
    return 1;
}

static auto l_vec3_dot(lua_State* L) -> int
{
    auto* a = l_check_vec3(L, 1);
    auto* b = l_check_vec3(L, 2);
    f32 d = a->x * b->x + a->y * b->y + a->z * b->z;
    lua_pushnumber(L, d);
    return 1;
}

static auto l_vec3_length(lua_State* L) -> int
{
    auto* v = l_check_vec3(L, 1);
    f32 len = std::sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    lua_pushnumber(L, len);
    return 1;
}

static auto l_vec3_normalize(lua_State* L) -> int
{
    auto* v = l_check_vec3(L, 1);
    f32 len = std::sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    if (len > 0.0f) {
        auto* r = static_cast<Vec3Userdata*>(lua_newuserdata(L, sizeof(Vec3Userdata)));
        r->x = v->x / len;
        r->y = v->y / len;
        r->z = v->z / len;
        luaL_setmetatable(L, "okn.vec3");
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

static auto l_vec3_tostring(lua_State* L) -> int
{
    auto* v = l_check_vec3(L, 1);
    lua_pushfstring(L, "vec3(%.4f, %.4f, %.4f)", v->x, v->y, v->z);
    return 1;
}

static auto l_vec3_index(lua_State* L) -> int
{
    auto* v = l_check_vec3(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (std::strcmp(key, "x") == 0) {
        lua_pushnumber(L, v->x);
    } else if (std::strcmp(key, "y") == 0) {
        lua_pushnumber(L, v->y);
    } else if (std::strcmp(key, "z") == 0) {
        lua_pushnumber(L, v->z);
    } else {
        luaL_getmetatable(L, "okn.vec3");
        lua_pushstring(L, key);
        lua_rawget(L, -2);
        lua_remove(L, -2);
    }
    return 1;
}

static auto l_vec3_newindex(lua_State* L) -> int
{
    auto* v = l_check_vec3(L, 1);
    const char* key = luaL_checkstring(L, 2);
    f32 value = static_cast<f32>(luaL_checknumber(L, 3));

    if (std::strcmp(key, "x") == 0) {
        v->x = value;
    } else if (std::strcmp(key, "y") == 0) {
        v->y = value;
    } else if (std::strcmp(key, "z") == 0) {
        v->z = value;
    }
    return 0;
}

// ── Entity ──

static auto l_entity_new(lua_State* L) -> int
{
    u64 id = static_cast<u64>(luaL_checkinteger(L, 1));
    auto* e = static_cast<EntityUserdata*>(lua_newuserdata(L, sizeof(EntityUserdata)));
    e->id = id;
    luaL_setmetatable(L, "okn.entity");
    return 1;
}

static auto l_entity_id(lua_State* L) -> int
{
    auto* e = static_cast<EntityUserdata*>(luaL_checkudata(L, 1, "okn.entity"));
    lua_pushinteger(L, static_cast<lua_Integer>(e->id));
    return 1;
}

static auto l_entity_is_valid(lua_State* L) -> int
{
    auto* e = static_cast<EntityUserdata*>(luaL_checkudata(L, 1, "okn.entity"));
    lua_pushboolean(L, e->id != 0 ? 1 : 0);
    return 1;
}

static auto l_entity_tostring(lua_State* L) -> int
{
    auto* e = static_cast<EntityUserdata*>(luaL_checkudata(L, 1, "okn.entity"));
    lua_pushfstring(L, "entity(%llu)", static_cast<unsigned long long>(e->id));
    return 1;
}

// ── World ──

static auto l_world_set_pointer(lua_State* L) -> int
{
    g_world_ptr = static_cast<void*>(lua_touserdata(L, 1));
    return 0;
}

static auto l_world_get_pointer(lua_State* L) -> int
{
    lua_pushlightuserdata(L, g_world_ptr);
    return 1;
}

static auto l_world_create_entity(lua_State* L) -> int
{
    u64 id = 1;
    lua_pushinteger(L, static_cast<lua_Integer>(id));
    return 1;
}

static auto l_world_destroy_entity(lua_State* L) -> int
{
    luaL_checkinteger(L, 1);
    return 0;
}

static auto l_world_add_component(lua_State* L) -> int
{
    (void)L;
    return 0;
}

static auto l_world_get_component(lua_State* L) -> int
{
    lua_pushnil(L);
    return 1;
}

// ── Input ──

static auto l_input_set_pointer(lua_State* L) -> int
{
    g_input_ptr = static_cast<void*>(lua_touserdata(L, 1));
    return 0;
}

static auto l_input_is_key_down(lua_State* L) -> int
{
    luaL_checkinteger(L, 1);
    lua_pushboolean(L, 0);
    return 1;
}

static auto l_input_mouse_position(lua_State* L) -> int
{
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    return 2;
}

} // anonymous namespace

// ── Register functions ──

auto EngineBindings::register_vec3(LuaContext& ctx) -> void
{
    auto* L = ctx.get_state();
    if (!L) return;

    luaL_newmetatable(L, "okn.vec3");

    lua_pushcfunction(L, l_vec3_add);
    lua_setfield(L, -2, "add");
    lua_pushcfunction(L, l_vec3_sub);
    lua_setfield(L, -2, "sub");
    lua_pushcfunction(L, l_vec3_mul);
    lua_setfield(L, -2, "mul");
    lua_pushcfunction(L, l_vec3_dot);
    lua_setfield(L, -2, "dot");
    lua_pushcfunction(L, l_vec3_length);
    lua_setfield(L, -2, "length");
    lua_pushcfunction(L, l_vec3_normalize);
    lua_setfield(L, -2, "normalize");

    lua_pushcfunction(L, l_vec3_add);
    lua_setfield(L, -2, "__add");
    lua_pushcfunction(L, l_vec3_sub);
    lua_setfield(L, -2, "__sub");
    lua_pushcfunction(L, l_vec3_mul);
    lua_setfield(L, -2, "__mul");
    lua_pushcfunction(L, l_vec3_index);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_vec3_newindex);
    lua_setfield(L, -2, "__newindex");
    lua_pushcfunction(L, l_vec3_tostring);
    lua_setfield(L, -2, "__tostring");

    lua_pop(L, 1);

    lua_newtable(L);
    lua_pushcfunction(L, l_vec3_new);
    lua_setfield(L, -2, "new");
    lua_setglobal(L, "vec3");
}

auto EngineBindings::register_entity(LuaContext& ctx) -> void
{
    auto* L = ctx.get_state();
    if (!L) return;

    luaL_newmetatable(L, "okn.entity");

    lua_pushcfunction(L, l_entity_id);
    lua_setfield(L, -2, "id");
    lua_pushcfunction(L, l_entity_is_valid);
    lua_setfield(L, -2, "is_valid");

    lua_pushcfunction(L, l_entity_id);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_entity_tostring);
    lua_setfield(L, -2, "__tostring");

    lua_pop(L, 1);

    lua_newtable(L);
    lua_pushcfunction(L, l_entity_new);
    lua_setfield(L, -2, "new");
    lua_setglobal(L, "entity");
}

auto EngineBindings::register_world(LuaContext& ctx) -> void
{
    auto* L = ctx.get_state();
    if (!L) return;

    lua_newtable(L);
    lua_pushcfunction(L, l_world_set_pointer);
    lua_setfield(L, -2, "set_pointer");
    lua_pushcfunction(L, l_world_get_pointer);
    lua_setfield(L, -2, "get_pointer");
    lua_pushcfunction(L, l_world_create_entity);
    lua_setfield(L, -2, "create_entity");
    lua_pushcfunction(L, l_world_destroy_entity);
    lua_setfield(L, -2, "destroy_entity");
    lua_pushcfunction(L, l_world_add_component);
    lua_setfield(L, -2, "add_component");
    lua_pushcfunction(L, l_world_get_component);
    lua_setfield(L, -2, "get_component");
    lua_setglobal(L, "world");
}

auto EngineBindings::register_input(LuaContext& ctx) -> void
{
    auto* L = ctx.get_state();
    if (!L) return;

    lua_newtable(L);
    lua_pushcfunction(L, l_input_set_pointer);
    lua_setfield(L, -2, "set_pointer");
    lua_pushcfunction(L, l_input_is_key_down);
    lua_setfield(L, -2, "is_key_down");
    lua_pushcfunction(L, l_input_mouse_position);
    lua_setfield(L, -2, "mouse_position");
    lua_setglobal(L, "input");
}

auto EngineBindings::register_all(LuaContext& ctx) -> void
{
    register_vec3(ctx);
    register_entity(ctx);
    register_world(ctx);
    register_input(ctx);
}

} // namespace okn::script

#endif