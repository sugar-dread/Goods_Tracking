project (SQLiteCpp)

set(SQLITECPP ${PROJECT_NAME})

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

set(SQLITECPP_SRC
    ${PROJECT_SOURCE_DIR}/Src/Backup.cpp
    ${PROJECT_SOURCE_DIR}/Src/Column.cpp
    ${PROJECT_SOURCE_DIR}/Src/Database.cpp
    ${PROJECT_SOURCE_DIR}/Src/Exception.cpp
    ${PROJECT_SOURCE_DIR}/Src/Savepoint.cpp
    ${PROJECT_SOURCE_DIR}/Src/Statement.cpp
    ${PROJECT_SOURCE_DIR}/Src/Transaction.cpp
)

source_group(Src FILES ${SQLITECPP_SRC})

set(SQLITECPP_INC
    ${PROJECT_SOURCE_DIR}/Inc/Assertion.h
    ${PROJECT_SOURCE_DIR}/Inc/Backup.h
    ${PROJECT_SOURCE_DIR}/Inc/Column.h
    ${PROJECT_SOURCE_DIR}/Inc/Database.h
    ${PROJECT_SOURCE_DIR}/Inc/Exception.h
    ${PROJECT_SOURCE_DIR}/Inc/ExecuteMany.h
    ${PROJECT_SOURCE_DIR}/Inc/Savepoint.h
    ${PROJECT_SOURCE_DIR}/Inc/Statement.h
    ${PROJECT_SOURCE_DIR}/Inc/Transaction.h
    ${PROJECT_SOURCE_DIR}/Inc/VariadicBind.h
    ${PROJECT_SOURCE_DIR}/Inc/SQLiteCpp.h
)

source_group(Inc FILES ${SQLITECPP_INC})
find_package(SQLite3 REQUIRED)

if(COMPILE_STATIC MATCHES ON)
    add_library(${PROJECT_NAME} ${SQLITECPP_SRC} ${SQLITECPP_INC} )
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME SQLiteCpp${VERSION}) 

else()

    add_library(${PROJECT_NAME} SHARED ${SQLITECPP_SRC} ${SQLITECPP_INC} )
    set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${VERSION} SOVERSION ${VERSION_MAJOR})

    include(GNUInstallDirs)
    install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} PERMISSIONS WORLD_READ)
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT headers FILES_MATCHING REGEX ".*\\.(hpp|h|hh)$")
    install(EXPORT ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(cmake/${PROJECT_NAME}ConfigVersion.cmake VERSION ${VERSION} COMPATIBILITY AnyNewerVersion)
    configure_package_config_file(cmake/${PROJECT_NAME}Config.cmake.in cmake/${PROJECT_NAME}Config.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

    add_custom_target("uninstall" COMMENT "Uninstall installed files")
    add_custom_command(
        TARGET "uninstall"
        POST_BUILD
        COMMENT "Uninstall files with install_manifest.txt"
        COMMAND xargs rm -vf < install_manifest.txt || echo Nothing in install_manifest.txt to be uninstalled!)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Inc>
    $<INSTALL_INTERFACE:include>)


include(${PROJECT_SOURCE_DIR}/cmake/SQLiteCppOptions.cmake)
target_link_libraries(${PROJECT_NAME} PUBLIC SQLite::SQLite3)


