project (GoodsTrack)

set(GoodsTrack_SRC ${PROJECT_SOURCE_DIR}/examples/main.cpp)
set(GoodsTrack "example")


add_executable(${GoodsTrack} ${GoodsTrack_SRC})

target_link_libraries(${GoodsTrack} ${QuizGeneratorCpp})

add_custom_target("run_${GoodsTrack}"
                  COMMENT "Runs the ${GoodsTrack} executable"
                  USES_TERMINAL
                  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${GoodsTrack} DEPENDS ${GoodsTrack})

    
