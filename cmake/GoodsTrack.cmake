project(GoodsTrack)

set(GoodsTrack ${PROJECT_NAME})

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

set(GoodsTrack_SRC
    ${PROJECT_SOURCE_DIR}/Src/goods_track.cpp
)

source_group(Src FILES ${GoodsTrack_SRC})

set(GoodsTrack_INC
    ${PROJECT_SOURCE_DIR}/Inc/goods_track.h
    ${PROJECT_SOURCE_DIR}/Inc/menu_utility.h
    ${PROJECT_SOURCE_DIR}/Inc/colorprint.h
)

source_group(Inc FILES ${GoodsTrack_INC})

add_library(${PROJECT_NAME} ${GoodsTrack_SRC} ${GoodsTrack_INC})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}${VERSION})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Inc>
    $<INSTALL_INTERFACE:include>)

find_package(RapidJSON REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

target_link_libraries(${PROJECT_NAME} SQLiteCpp rapidjson OpenSSL::Crypto curl)
