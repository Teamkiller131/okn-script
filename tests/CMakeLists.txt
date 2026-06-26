cmake_minimum_required(VERSION 3.20)

set(TESTS_TARGET okn-script_tests)

file(GLOB_RECURSE ${TESTS_TARGET}_SRC
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

if(${TESTS_TARGET}_SRC)
    add_executable(${TESTS_TARGET} ${${TESTS_TARGET}_SRC})
    target_link_libraries(${TESTS_TARGET} PRIVATE okn-script)
    target_compile_features(${TESTS_TARGET} PRIVATE cxx_std_23)

    # sol2-based engine-binding test (real Lua usertypes).
    find_package(sol2 CONFIG QUIET)
    find_package(Lua QUIET)
    if(sol2_FOUND AND LUA_FOUND)
        # okn-ecs for the ScriptingBridge <-> sol2 binding test (test_ecs_binding.cpp).
        target_link_libraries(${TESTS_TARGET} PRIVATE sol2::sol2 okn-ecs)
        target_compile_definitions(${TESTS_TARGET} PRIVATE OKN_SCRIPT_HAS_SOL2=1)
        if(MSVC)
            set_source_files_properties(
                "${CMAKE_CURRENT_SOURCE_DIR}/test_binding.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/test_ecs_binding.cpp"
                PROPERTIES COMPILE_OPTIONS "/WX-")
        endif()
    endif()

    add_test(NAME ${TESTS_TARGET} COMMAND ${TESTS_TARGET})
endif()