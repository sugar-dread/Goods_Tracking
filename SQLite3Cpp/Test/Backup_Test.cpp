/**
 * @file Backup_Test.cpp
 * @author your name (you@domain.com)
 * @brief unit test file of SQLite Backup
 * @version 0.1
 * @date 2022-11-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Backup.h"
#include "Database.h"
#include "Statement.h"
#include "Exception.h"

#include <sqlite3.h>  // for SQLITE_ERROR, SQLITE_RANGE and SQLITE_DONE

#include <gtest/gtest.h>

#include <cstdio>

TEST( Backup, initExceptionTest ) {
  remove( "backup_test.db3" );
  {
    SQLite::Database srcDB { "backup_test.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE };
    srcDB.exec( "CREATE TABLE backup_test (id INTEGER PRIMARY KEY, value TEXT)" );
    ASSERT_EQ( 1, srcDB.exec( "INSERT INTO backup_test VALUES (1, \"first\")" ) );
    ASSERT_EQ( 1, srcDB.exec( "INSERT INTO backup_test VALUES (2, \"second\")" ) );
    EXPECT_THROW( SQLite::Backup backup( srcDB, srcDB ), SQLite::Exception );
    EXPECT_THROW( SQLite::Backup backup( srcDB, "main", srcDB, "main" ), SQLite::Exception );
    const std::string name( "main" );
    EXPECT_THROW( SQLite::Backup backup( srcDB, name, srcDB, name ), SQLite::Exception );
  }
  remove( "backup_test.db3" );
}

TEST( Backup, executeStepTest ) {
  remove( "backup_test.db3" );
  remove( "backup_test.db3.backup" );
  {
    SQLite::Database srcDB( "backup_test.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE );
    srcDB.exec( "CREATE TABLE backup_test (id INTEGER PRIMARY KEY, value TEXT)" );
    ASSERT_EQ( 1, srcDB.exec( "INSERT INTO backup_test VALUES (1, \"first\")" ) );
    ASSERT_EQ( 1, srcDB.exec( "INSERT INTO backup_test VALUES (2, \"second\")" ) );

    SQLite::Database destDB( "backup_test.db3.backup", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE );
    SQLite::Backup backup( destDB, "main", srcDB, "main" );
    int res = backup.executeStep( 1 );  // backup only one page at a time
    ASSERT_EQ( SQLite::OK, res );
    const int total = backup.getTotalPageCount();
    ASSERT_EQ( 2, total );
    int remaining = backup.getRemainingPageCount();
    ASSERT_EQ( 1, remaining );
    res = backup.executeStep( 1 );  // backup the second and last page
    ASSERT_EQ( SQLITE_DONE, res );
    remaining = backup.getRemainingPageCount();
    ASSERT_EQ( 0, remaining );

    SQLite::Statement query { destDB, "SELECT * FROM backup_test ORDER BY id ASC" };
    ASSERT_TRUE( query.executeStep() );
    EXPECT_EQ( 1, query.getColumn( 0 ).getInt() );
    EXPECT_STREQ( "first", query.getColumn( 1 ) );
    ASSERT_TRUE( query.executeStep() );
    EXPECT_EQ( 2, query.getColumn( 0 ).getInt() );
    EXPECT_STREQ( "second", query.getColumn( 1 ) );
  }
  remove( "backup_test.db3" );
  remove( "backup_test.db3.backup" );
}

TEST( Backup, executeStepAllTest ) {
  remove( "backup_test.db3" );
  remove( "backup_test.db3.backup" );
  {
    SQLite::Database srcDB( "backup_test.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE );
    srcDB.exec( "CREATE TABLE backup_test (id INTEGER PRIMARY KEY, value TEXT)" );
    ASSERT_EQ( 1, srcDB.exec( "INSERT INTO backup_test VALUES (1, \"first\")" ) );
    ASSERT_EQ( 1, srcDB.exec( "INSERT INTO backup_test VALUES (2, \"second\")" ) );

    SQLite::Database destDB( "backup_test.db3.backup", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE );
    SQLite::Backup backup( destDB, srcDB );
    const int res = backup.executeStep();  // uses default argument "-1" => execute all steps at once
    ASSERT_EQ( SQLITE_DONE, res );
    const int total = backup.getTotalPageCount();
    ASSERT_EQ( 2, total );
    const int remaining = backup.getRemainingPageCount();
    ASSERT_EQ( 0, remaining );

    SQLite::Statement query { destDB, "SELECT * FROM backup_test ORDER BY id ASC" };
    ASSERT_TRUE( query.executeStep() );
    EXPECT_EQ( 1, query.getColumn( 0 ).getInt() );
    EXPECT_STREQ( "first", query.getColumn( 1 ) );
    ASSERT_TRUE( query.executeStep() );
    EXPECT_EQ( 2, query.getColumn( 0 ).getInt() );
    EXPECT_STREQ( "second", query.getColumn( 1 ) );
  }
  remove( "backup_test.db3" );
  remove( "backup_test.db3.backup" );
}

TEST( Backup, executeStepException ) {
  remove( "backup_test.db3" );
  remove( "backup_test.db3.backup" );
  {
    SQLite::Database srcDB { "backup_test.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE };
    srcDB.exec( "CREATE TABLE backup_test (id INTEGER PRIMARY KEY, value TEXT)" );
    ASSERT_EQ( 1, srcDB.exec( "INSERT INTO backup_test VALUES (1, \"first\")" ) );
    ASSERT_EQ( 1, srcDB.exec( "INSERT INTO backup_test VALUES (2, \"second\")" ) );
    {
      SQLite::Database destDB( "backup_test.db3.backup", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE );
      (void)destDB;
    }
    {
      SQLite::Database destDB( "backup_test.db3.backup", SQLite::OPEN_READONLY );
      SQLite::Backup backup( destDB, srcDB );
      EXPECT_THROW( backup.executeStep(), SQLite::Exception );
    }
  }
  remove( "backup_test.db3" );
  remove( "backup_test.db3.backup" );
}