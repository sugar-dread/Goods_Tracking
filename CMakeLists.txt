cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Project name and a few useful settings. Other commands can pick up the results
project(
  GoodsTrack
  VERSION 1.0
  DESCRIPTION "Goods are stored in database. Prices are read by Yahoo Finance API"
  LANGUAGES CXX)

message(STATUS "Using C++ standard c++${CMAKE_CXX_STANDARD}")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "CMAKE_CXX_COMPILER '${CMAKE_CXX_COMPILER}' '${CMAKE_CXX_COMPILER_ID}' '${CMAKE_CXX_COMPILER_VERSION}'")
message(STATUS "CMAKE_CXX_FLAGS '${CMAKE_CXX_FLAGS}'")

message(STATUS "Adding SQLiteCpp library to project with subdirectory")
set(COMPILE_STATIC ON)
set(BUILD_TESTS OFF)
set(SQLITECPP_EXAMPLE_NAME "")
add_subdirectory(SQLite3Cpp)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/GoodsTrack.cmake)
set(BUILD_TESTS "compile GoodsTrack with tests." ON)

if(BUILD_TESTS MATCHES ON)
  include(${CMAKE_CURRENT_LIST_DIR}/cmake/GoodsTrackTest.cmake)
endif()

set(MAKE_EXAMPLE "compile GoodsTrack with example." ON)

if(MAKE_EXAMPLE MATCHES ON)
  include(${CMAKE_CURRENT_LIST_DIR}/cmake/GoodsTrackExample.cmake)
endif()

set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)

  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output." ON)

if(FORCE_COLORED_OUTPUT)
  add_compile_options(-fdiagnostics-color=always)
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)

  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output." ON)

if(FORCE_COLORED_OUTPUT)
  add_compile_options(-fdiagnostics-color=always)
endif()

option(GoodsTrack_RUN_DOXYGEN "Run Doxygen C++ documentation tool." ON)

if(GoodsTrack_RUN_DOXYGEN)
  find_package(Doxygen)

  if(DOXYGEN_FOUND)
    # add a Doxygen target to the "all" target
    message(STATUS "DOXYGEN_FOUND")
    add_custom_target(GoodsTrack_doxygen ALL COMMAND doxygen Doxyfile > /dev/null WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
  endif(DOXYGEN_FOUND)
else(GoodsTrack_RUN_DOXYGEN)
  message(STATUS "GoodsTrack_RUN_DOXYGEN OFF")
endif(GoodsTrack_RUN_DOXYGEN)