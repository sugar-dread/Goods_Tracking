cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Project name and a few useful settings. Other commands can pick up the results
project(
  SQLiteCpp
  VERSION 1.0
  DESCRIPTION "SQLite Cpp wrapper library"
  LANGUAGES CXX)

message(STATUS "Using C++ standard c++${CMAKE_CXX_STANDARD}")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "CMAKE_CXX_COMPILER '${CMAKE_CXX_COMPILER}' '${CMAKE_CXX_COMPILER_ID}' '${CMAKE_CXX_COMPILER_VERSION}'")
message(STATUS "CMAKE_CXX_FLAGS '${CMAKE_CXX_FLAGS}'")

option(COMPILE_STATIC "compile SQLiteCpp library as a static." ON)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/SQLiteCpp.cmake)

option(BUILD_TESTS "compile SQLiteCpp with tests." OFF)
if(BUILD_TESTS MATCHES ON)
  include(${CMAKE_CURRENT_LIST_DIR}/cmake/SQLiteCppTest.cmake)
endif()

set(SQLITECPP_EXAMPLE_NAME "example1")
if(SQLITECPP_EXAMPLE_NAME)
  include(${CMAKE_CURRENT_LIST_DIR}/cmake/SQLiteCppExamples.cmake)
endif()

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output." ON)
if (FORCE_COLORED_OUTPUT)
    add_compile_options (-fdiagnostics-color=always)
endif ()


if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output." ON)
if (FORCE_COLORED_OUTPUT)
    add_compile_options (-fdiagnostics-color=always)
endif ()


option(SQLITECPP_RUN_DOXYGEN "Run Doxygen C++ documentation tool." ON)
if (SQLITECPP_RUN_DOXYGEN)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # add a Doxygen target to the "all" target
        add_custom_target(SQLiteCpp_doxygen ALL COMMAND doxygen Doxyfile > /dev/null WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    endif (DOXYGEN_FOUND)
else (SQLITECPP_RUN_DOXYGEN)
    message(STATUS "SQLITECPP_RUN_DOXYGEN OFF")
endif (SQLITECPP_RUN_DOXYGEN)