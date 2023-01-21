project (SQLiteCppExamples)

set(SQLITECPPEXAMPLE1_SRC ${PROJECT_SOURCE_DIR}/examples/example1/main.cpp)
source_group(example1 FILES ${SQLITECPPEXAMPLE1_SRC})

set(SQLITECPPEXAMPLE2_SRC ${PROJECT_SOURCE_DIR}/examples/example2/main.cpp)
source_group(example2 FILES ${SQLITECPPEXAMPLE2_SRC})

if(SQLITECPP_EXAMPLE_NAME STREQUAL "example1")
    add_executable(${SQLITECPP_EXAMPLE_NAME} ${SQLITECPPEXAMPLE1_SRC})
elseif(SQLITECPP_EXAMPLE_NAME STREQUAL "example2")
    add_executable(${SQLITECPP_EXAMPLE_NAME} ${SQLITECPPEXAMPLE2_SRC})
endif()

target_link_libraries(${SQLITECPP_EXAMPLE_NAME} ${SQLITECPP})

add_custom_target("run_${SQLITECPP_EXAMPLE_NAME}"
                  COMMENT "Runs the ${PROJECT_NAME} executable"
                  USES_TERMINAL
                  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SQLITECPP_EXAMPLE_NAME} DEPENDS ${SQLITECPP_EXAMPLE_NAME})
