cmake_minimum_required(VERSION 3.20)
project(okn-script LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(BUILD_SAMPLES "Build samples" OFF)
option(BUILD_TESTS   "Build tests"   ON)

# Feature toggles
option(OKN_SCRIPT_ENABLE_LUA         "Enable Lua runtime" ON)
option(OKN_SCRIPT_ENABLE_JS          "Enable JavaScript runtime" ON)
option(OKN_SCRIPT_ENABLE_PYTHON      "Enable Python runtime" ON)
option(OKN_SCRIPT_ENABLE_HOT_RELOAD  "Enable script hot-reload" ON)
option(OKN_SCRIPT_ENABLE_SANDBOX     "Enable sandbox policies/quotas" ON)
option(OKN_SCRIPT_ENABLE_DEBUGGER    "Enable debugger/protocol server" ON)
option(OKN_SCRIPT_ENABLE_PROFILE     "Enable profiling hooks" ON)
option(OKN_SCRIPT_ENABLE_CLI         "Enable CLI tools" ON)
option(OKN_SCRIPT_ENABLE_MEMORY_HOOKS "Enable memory allocator hooks" ON)

file(GLOB_RECURSE ${PROJECT_NAME}_SRC
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Lua runtime
find_package(Lua QUIET)
if(LUA_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${LUA_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${LUA_LIBRARIES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC OKN_SCRIPT_HAS_LUA=1)
endif()

# Dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC okn-core okn-platform)

# Compile definitions
if(OKN_SCRIPT_ENABLE_LUA)
    target_compile_definitions(${PROJECT_NAME} PUBLIC OKN_SCRIPT_ENABLE_LUA=1)
endif()
if(OKN_SCRIPT_ENABLE_JS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC OKN_SCRIPT_ENABLE_JS=1)
endif()
if(OKN_SCRIPT_ENABLE_PYTHON)
    target_compile_definitions(${PROJECT_NAME} PUBLIC OKN_SCRIPT_ENABLE_PYTHON=1)
endif()
if(OKN_SCRIPT_ENABLE_HOT_RELOAD)
    target_compile_definitions(${PROJECT_NAME} PUBLIC OKN_SCRIPT_ENABLE_HOT_RELOAD=1)
endif()
if(OKN_SCRIPT_ENABLE_SANDBOX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC OKN_SCRIPT_ENABLE_SANDBOX=1)
endif()
if(OKN_SCRIPT_ENABLE_DEBUGGER)
    target_compile_definitions(${PROJECT_NAME} PUBLIC OKN_SCRIPT_ENABLE_DEBUGGER=1)
endif()
if(OKN_SCRIPT_ENABLE_PROFILE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC OKN_SCRIPT_ENABLE_PROFILE=1)
endif()
if(OKN_SCRIPT_ENABLE_CLI)
    target_compile_definitions(${PROJECT_NAME} PUBLIC OKN_SCRIPT_ENABLE_CLI=1)
endif()
if(OKN_SCRIPT_ENABLE_MEMORY_HOOKS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC OKN_SCRIPT_ENABLE_MEMORY_HOOKS=1)
endif()

# Samples
if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()