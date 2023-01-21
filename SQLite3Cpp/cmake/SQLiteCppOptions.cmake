# Options relative to SQLite and SQLiteC++ functions

option(SQLITE_ENABLE_COLUMN_METADATA "Enable Column::getColumnOriginName(). Require support from sqlite3 library." ON)
if (SQLITE_ENABLE_COLUMN_METADATA)
    # Enable the use of SQLite column metadata and Column::getColumnOriginName() method,
    # Require that the sqlite3 library is also compiled with this flag (default under Debian/Ubuntu, but not on Mac OS X).
    target_compile_definitions(${SQLITECPP} PUBLIC SQLITE_ENABLE_COLUMN_METADATA)
endif (SQLITE_ENABLE_COLUMN_METADATA)

option(SQLITE_ENABLE_ASSERT_HANDLER "Enable the user definition of a assertion_failed() handler." OFF)
if (SQLITE_ENABLE_ASSERT_HANDLER)
    # Enable the user definition of a assertion_failed() handler (default to false, easier to handler for beginners).
    target_compile_definitions(${SQLITECPP} PUBLIC SQLITECPP_ENABLE_ASSERT_HANDLER)
endif (SQLITE_ENABLE_ASSERT_HANDLER)

option(SQLITE_HAS_CODEC "Enable database encryption API. Not available in the public release of SQLite." OFF)
if (SQLITE_HAS_CODEC)
    # Enable database encryption API. Requires implementations of sqlite3_key & sqlite3_key_v2.
    # Eg. SQLCipher (libsqlcipher-dev) is an SQLite extension that provides 256 bit AES encryption of database files. 
    target_compile_definitions(${SQLITECPP} PUBLIC SQLITE_HAS_CODEC)
endif (SQLITE_HAS_CODEC)

option(SQLITE_USE_LEGACY_STRUCT "Fallback to forward declaration of legacy struct sqlite3_value (pre SQLite 3.19)" OFF)
if (SQLITE_USE_LEGACY_STRUCT)
    # Force forward declaration of legacy struct sqlite3_value (pre SQLite 3.19)
    target_compile_definitions(${SQLITECPP} PUBLIC SQLITE_USE_LEGACY_STRUCT)
endif (SQLITE_USE_LEGACY_STRUCT)

option(SQLITE_OMIT_LOAD_EXTENSION "Enable omit load extension" OFF)
if (SQLITE_OMIT_LOAD_EXTENSION)
    # Enable the user definition of load_extension().
    target_compile_definitions(${SQLITECPP} PUBLIC SQLITE_OMIT_LOAD_EXTENSION)
endif (SQLITE_OMIT_LOAD_EXTENSION)


option(SQLITE_OMIT_LOAD_EXTENSION "Enable omit load extension" OFF)
if (SQLITE_OMIT_LOAD_EXTENSION)
    # Enable the user definition of load_extension().
    target_compile_definitions(${SQLITECPP} PUBLIC SQLITE_OMIT_LOAD_EXTENSION)
endif (SQLITE_OMIT_LOAD_EXTENSION)


if (UNIX AND (CMAKE_COMPILER_IS_GNUCXX OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"))
    set_target_properties(${SQLITECPP} PROPERTIES COMPILE_FLAGS "-fPIC")
endif (UNIX AND (CMAKE_COMPILER_IS_GNUCXX OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"))


option(SQLITECPP_USE_ASAN "Use Address Sanitizer." OFF)
if (SQLITECPP_USE_ASAN)
    if ((CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 6) OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
        message (STATUS "Using Address Sanitizer")
        set_target_properties(${SQLITECPP} PROPERTIES COMPILE_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
        if (CMAKE_COMPILER_IS_GNUCXX)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
        endif ()
    endif ()
endif (SQLITECPP_USE_ASAN)


if (SQLITECPP_USE_GCOV)
    # Prevent the compiler from removing the unused inline functions so that they get tracked as "non-covered"
    set_target_properties(SQLiteCpp PROPERTIES COMPILE_FLAGS "-fkeep-inline-functions -fkeep-static-functions")
endif ()

## Build provided copy of SQLite3 C library ##

option(SQLITECPP_INTERNAL_SQLITE "Add the internal SQLite3 source to the project." OFF)
if (SQLITECPP_INTERNAL_SQLITE)
    message(STATUS "Compile sqlite3 from source in subdirectory")
    option(SQLITE_ENABLE_JSON1 "Enable JSON1 extension when building internal sqlite3 library." ON)
    # build the SQLite3 C library (for ease of use/compatibility) versus Linux sqlite3-dev package
    add_subdirectory(sqlite3)
    target_link_libraries(SQLiteCpp PUBLIC SQLite::SQLite3)
else (SQLITECPP_INTERNAL_SQLITE)
    # When using the SQLite codec, we need to link against the sqlcipher lib & include <sqlcipher/sqlite3.h>
    # So this gets the lib & header, and links/includes everything
    if(SQLITE_HAS_CODEC)
        # Make PkgConfig optional since Windows doesn't usually have it installed.
        find_package(PkgConfig QUIET)
        if(PKG_CONFIG_FOUND)
            # IMPORTED_TARGET was added in 3.6.3
            if(CMAKE_VERSION VERSION_LESS 3.6.3)
                pkg_check_modules(sqlcipher REQUIRED sqlcipher)
                # Only used in Database.cpp so PRIVATE to hide from end-user
                # Since we can't use IMPORTED_TARGET on this older Cmake version, manually link libs & includes
                target_link_libraries(SQLiteCpp PRIVATE ${sqlcipher_LIBRARIES})
                target_include_directories(SQLiteCpp PRIVATE ${sqlcipher_INCLUDE_DIRS})
            else()
                pkg_check_modules(sqlcipher REQUIRED IMPORTED_TARGET sqlcipher)
                # Only used in Database.cpp so PRIVATE to hide from end-user
                target_link_libraries(SQLiteCpp PRIVATE PkgConfig::sqlcipher)
            endif()
        else()
            # Since we aren't using pkgconf here, find it manually
            find_library(sqlcipher_LIBRARY "sqlcipher")
            find_path(sqlcipher_INCLUDE_DIR "sqlcipher/sqlite3.h"
                PATH_SUFFIXES
                    "include"
                    "includes"
            )
            # Hides it from the GUI
            mark_as_advanced(sqlcipher_LIBRARY sqlcipher_INCLUDE_DIR)
            if(NOT sqlcipher_INCLUDE_DIR)
                message(FATAL_ERROR "${PROJECT_NAME} requires the \"<sqlcipher/sqlite3.h>\" header to use the codec functionality but it wasn't found.")
            elseif(NOT sqlcipher_LIBRARY)
                message(FATAL_ERROR "${PROJECT_NAME} requires the sqlcipher library to use the codec functionality but it wasn't found.")
            endif()
            # Only used in Database.cpp so PRIVATE to hide from end-user
            target_include_directories(SQLiteCpp PRIVATE "${sqlcipher_INCLUDE_DIR}/sqlcipher")
            target_link_libraries(SQLiteCpp PRIVATE ${sqlcipher_LIBRARY})
        endif()
    else()
        find_package (SQLite3 REQUIRED)
        message(STATUS "Link to sqlite3 system library")
        target_link_libraries(SQLiteCpp PUBLIC SQLite::SQLite3)
        if(SQLite3_VERSION VERSION_LESS "3.19")
            set_target_properties(SQLiteCpp PROPERTIES COMPILE_FLAGS "-DSQLITECPP_HAS_MEM_STRUCT")
        endif()
    endif()
endif (SQLITECPP_INTERNAL_SQLITE)


# Link target with pthread and dl for Unix
if (UNIX)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(SQLiteCpp PUBLIC Threads::Threads ${CMAKE_DL_LIBS})
endif (UNIX)