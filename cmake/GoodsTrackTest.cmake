project(GoodsTrackTests)

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output." ON)

if(FORCE_COLORED_OUTPUT)
    add_compile_options(-fdiagnostics-color=always)
endif()

set(GoodsTrack_TESTS
    ${PROJECT_SOURCE_DIR}/Test/Goods_Test.cpp
    ${PROJECT_SOURCE_DIR}/Test/webConn_Test.cpp
    ${PROJECT_SOURCE_DIR}/Test/GoodsTrack_Test.cpp
)

source_group(Test FILES ${GoodsTrack_TESTS})
add_executable(${PROJECT_NAME} ${GoodsTrack_TESTS})

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} ${GoodsTrack} gtest gtest_main Threads::Threads)

enable_testing()
include(GoogleTest)

gtest_discover_tests(${PROJECT_NAME})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
set(CMAKE_BUILD_TYPE Debug)

include(${PROJECT_SOURCE_DIR}/cmake/CodeCoverage.cmake)
append_coverage_compiler_flags()

setup_target_for_coverage_gcovr_html(NAME ${PROJECT_NAME}Html EXECUTABLE ${PROJECT_NAME} BASE_DIRECTORY ${PROJECT_SOURCE_DIR}/Src)

add_custom_target("runTest"
    COMMENT "Runs the ${PROJECT_NAME} executable"
    USES_TERMINAL
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} DEPENDS ${PROJECT_NAME})
