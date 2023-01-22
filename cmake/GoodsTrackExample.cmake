project(GoodsTrackExample)

set(GoodsTrack_SRC ${PROJECT_SOURCE_DIR}/examples/main.cpp)
set(GoodsTrackExample "example")

add_executable(${GoodsTrackExample} ${GoodsTrack_SRC})

target_link_libraries(${GoodsTrackExample} ${GoodsTrack})

add_custom_target("run_${GoodsTrackExample}"
    COMMENT "Runs the ${GoodsTrackExample} executable"
    USES_TERMINAL
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${GoodsTrackExample} DEPENDS ${GoodsTrackExample})
