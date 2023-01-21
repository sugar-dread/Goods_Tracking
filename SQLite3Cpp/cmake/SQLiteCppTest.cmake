project (SQLiteCppUnitTests)

option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output." ON)
if (FORCE_COLORED_OUTPUT)
    add_compile_options (-fdiagnostics-color=always)
endif ()


set(SQLITECPP_TESTS
    ${PROJECT_SOURCE_DIR}/Test/Backup_Test.cpp
    ${PROJECT_SOURCE_DIR}/Test/Column_Test.cpp
    ${PROJECT_SOURCE_DIR}/Test/Database_Test.cpp
    ${PROJECT_SOURCE_DIR}/Test/Exception_Test.cpp
    ${PROJECT_SOURCE_DIR}/Test/ExecuteMany_Test.cpp
    ${PROJECT_SOURCE_DIR}/Test/Savepoint_Test.cpp
    ${PROJECT_SOURCE_DIR}/Test/Statement_Test.cpp
    ${PROJECT_SOURCE_DIR}/Test/Transaction_Test.cpp
    ${PROJECT_SOURCE_DIR}/Test/VariadicBind_Test.cpp
)

source_group(Test FILES ${SQLITECPP_TESTS})
add_executable(${PROJECT_NAME} ${SQLITECPP_TESTS})
target_link_libraries(${PROJECT_NAME} ${SQLITECPP} gtest gtest_main)

# Include Google Test
enable_testing()
include(GoogleTest)

gtest_discover_tests(${PROJECT_NAME})

# GCC GCOV code coverage tool settings
# turn off the optimization for non-skewed coverage reports
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
set(CMAKE_BUILD_TYPE Debug)

# include CodeCoverage.cmake
list(APPEND CMAKE_MODULE_PATH "/lib/cmake")
include(CodeCoverage)
append_coverage_compiler_flags()

# excludes requested files. We may also use wildcards "dir/*"
set(COVERAGE_EXCLUDES "${CMAKE_SOURCE_DIR}/*")

add_custom_target("runTest"
                  COMMENT "Runs the ${PROJECT_NAME} executable"
                  USES_TERMINAL
                  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} DEPENDS ${PROJECT_NAME})

