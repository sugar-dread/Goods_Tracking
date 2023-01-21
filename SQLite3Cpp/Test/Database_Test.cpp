/**
 * @file Database_Test.cpp
 * @author your name (you@domain.com)
 * @brief unit test file for database class
 * @version 0.1
 * @date 2022-11-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Database.h"

#include <gtest/gtest.h>

#include <sqlite3.h>
#include <filesystem>
#include <cstdio>
#include <fstream>

#ifdef SQLITECPP_ENABLE_ASSERT_HANDLER
namespace SQLite {
/// definition of the assertion handler enabled when SQLITECPP_ENABLE_ASSERT_HANDLER is defined in the project
/// (CMakeList.txt)
void assertion_failed( const char* apFile, const long apLine, const char* apFunc, const char* apExpr,
                       const char* apMsg ) {
  // TODO: unit test that this assertion callback get called (already tested manually)
  std::cout << "assertion_failed(" << apFile << ", " << apLine << ", " << apFunc << ", " << apExpr << ", " << apMsg
            << ")\n";
}
}  // namespace SQLite
#endif

SQLite::Database DatabaseBuilder( const char* name ) {
  return SQLite::Database( name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE );
}

TEST( SQLiteCpp, version ) {
  EXPECT_STREQ( SQLITE_VERSION, SQLite::VERSION );
  EXPECT_EQ( SQLITE_VERSION_NUMBER, SQLite::VERSION_NUMBER );
  EXPECT_STREQ( SQLITE_VERSION, SQLite::getLibVersion() );
  EXPECT_EQ( SQLITE_VERSION_NUMBER, SQLite::getLibVersionNumber() );
}

TEST( Database, CtorExecCreateDropExist ) {
  remove( "test.db3" );
  {
    // Try to open a non-existing database
    std::string filename { "test.db3" };
    // Default flag is READ_ONLY where db should be exists but theere is any file called test.db3
    EXPECT_THROW( SQLite::Database not_found( filename ), SQLite::Exception );

    // Create a new database
    SQLite::Database db( std::filesystem::path( "test.db3" ), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE );
    EXPECT_STREQ( "test.db3", db.getFilename().c_str() );
    EXPECT_FALSE( db.tableExists( "test" ) );
    EXPECT_FALSE( db.tableExists( std::string { "test" } ) );
    EXPECT_EQ( 0, db.getLastInsertRowid() );

    EXPECT_EQ( 0, db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" ) );
    EXPECT_TRUE( db.tableExists( "test" ) );
    EXPECT_TRUE( db.tableExists( std::string { "test" } ) );
    EXPECT_EQ( 0, db.getLastInsertRowid() );

    EXPECT_EQ( 0, db.exec( "DROP TABLE IF EXISTS test" ) );
    EXPECT_FALSE( db.tableExists( "test" ) );
    EXPECT_FALSE( db.tableExists( std::string { "test" } ) );
    EXPECT_EQ( 0, db.getLastInsertRowid() );
  }
  remove( "test.db3" );
}

TEST( Database, MoveCtorTest ) {
  remove( "test.db3" );
  {
    SQLite::Database db = DatabaseBuilder( "test.db3" );
    EXPECT_FALSE( db.tableExists( "test" ) );
    EXPECT_TRUE( db.getHandle() != nullptr );
    SQLite::Database moved = std::move( db );
    EXPECT_TRUE( db.getHandle() == nullptr );
    EXPECT_TRUE( moved.getHandle() != nullptr );
    EXPECT_FALSE( moved.tableExists( "test" ) );
  }
  remove( "test.db3" );
}

TEST( Database, createCloseReopen ) {
  remove( "test.db3" );
  {
    // Try to open the non-existing database
    EXPECT_THROW( SQLite::Database not_found( "test.db3" ), SQLite::Exception );

    SQLite::Database db = DatabaseBuilder( "test.db3" );
    EXPECT_FALSE( db.tableExists( "test" ) );
    db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" );
    EXPECT_TRUE( db.tableExists( "test" ) );
  }
  {
    // reopen data base file
    SQLite::Database db( "test.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE );
    EXPECT_TRUE( db.tableExists( "test" ) );
  }
  remove( "test.db3" );
}

TEST( Database, inMemory ) {
  {
    // Create a new database
    SQLite::Database db( ":memory:", SQLite::OPEN_READWRITE );
    EXPECT_FALSE( db.tableExists( "test" ) );
    db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" );
    EXPECT_TRUE( db.tableExists( "test" ) );
    // Create a new database: not shared with the above db
    SQLite::Database db2( ":memory:" );
    EXPECT_FALSE( db2.tableExists( "test" ) );
  }  // Close an destroy DBs
  {
    // Create a new database: no more "test" table
    SQLite::Database db( ":memory:" );
    EXPECT_FALSE( db.tableExists( "test" ) );
  }  // Close an destroy DB
}

TEST( Database, BackupTest ) {
  SQLite::Database db( ":memory:", SQLite::OPEN_READWRITE );
  EXPECT_FALSE( db.tableExists( "test" ) );
  db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" );
  EXPECT_TRUE( db.tableExists( "test" ) );

  remove( "backup.db3" );
  EXPECT_NO_THROW( db.backup( "backup.db3", SQLite::Database::BackupType::Save ) );
  // Trash the table
  db.exec( "DROP TABLE test" );
  EXPECT_FALSE( db.tableExists( "test" ) );

  // Import the data back from the file
  EXPECT_NO_THROW( db.backup( "backup.db3", SQLite::Database::BackupType::Load ) );
  remove( "backup.db3" );

  EXPECT_TRUE( db.tableExists( "test" ) );
}

#if SQLITE_VERSION_NUMBER >= 3007015  // SQLite v.3.7.15 is first version with PRAGMA busy_timeout
TEST( Database, BusyTimeoutTest ) {
  {
    SQLite::Database db( ":memory:" );
    // Busy timeout default to 0ms: any contention between threads or process leads to SQLITE_BUSY error
    EXPECT_EQ( 0, db.execAndGet( "PRAGMA busy_timeout" ).getInt() );

    // Set a non null busy timeout: any contention between threads will leads to as much retry as possible during the
    // time
    db.setBusyTimeout( 5000 );
    EXPECT_EQ( 5000, db.execAndGet( "PRAGMA busy_timeout" ).getInt() );
    // rese timeout to 0
    db.setBusyTimeout( 0 );
    EXPECT_EQ( 0, db.execAndGet( "PRAGMA busy_timeout" ).getInt() );
  }
  {
    SQLite::Database db( ":memory:", SQLite::OPEN_READWRITE, 5000 );
    EXPECT_EQ( 5000, db.execAndGet( "PRAGMA busy_timeout" ).getInt() );
    // Reset timeout to null
    db.setBusyTimeout( 0 );
    EXPECT_EQ( 0, db.execAndGet( "PRAGMA busy_timeout" ).getInt() );
  }
}
#endif

TEST( Database, exec ) {
  SQLite::Database db( ":memory:", SQLite::OPEN_READWRITE );
  // Create a new table with an explicit "id" column aliasing the underlying rowid
  //  NOTE: here exec() returns 0 only because it is the first statement since database connection
  //  but its return is an undefined value for "CREATE TABLE" statements.
  db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" );
  EXPECT_EQ( 0, db.getChanges() );
  EXPECT_EQ( 0, db.getLastInsertRowid() );
  EXPECT_EQ( 0, db.getTotalChanges() );

  // first row : insert the "first" text value into new row of id 1
  EXPECT_EQ( 1, db.exec( "INSERT INTO test VALUES(NULL, \"first\")" ) );
  EXPECT_EQ( 1, db.getChanges() );
  EXPECT_EQ( 1, db.getLastInsertRowid() );
  EXPECT_EQ( 1, db.getTotalChanges() );

  // second row : insert the "second" text value into new row of id 2
  EXPECT_EQ( 1, db.exec( "INSERT INTO test VALUES(NULL, \"second\")" ) );
  EXPECT_EQ( 1, db.getChanges() );
  EXPECT_EQ( 2, db.getLastInsertRowid() );
  EXPECT_EQ( 2, db.getTotalChanges() );

  // third row: insert the "third" text value into new row of id 3
  const std::string insert { "INSERT INTO test VALUES (NULL, \"third\")" };
  EXPECT_EQ( 1, db.exec( insert ) );
  EXPECT_EQ( 1, db.getChanges() );
  EXPECT_EQ( 3, db.getLastInsertRowid() );
  EXPECT_EQ( 3, db.getTotalChanges() );

  // update the second row : update text value to "second_update"
  EXPECT_EQ( 1, db.exec( "UPDATE test SET value=\"second-updated\" WHERE id='2'" ) );
  EXPECT_EQ( 1, db.getChanges() );
  EXPECT_EQ( 3, db.getLastInsertRowid() );  // last inserted row ID is still 3
  EXPECT_EQ( 4, db.getTotalChanges() );

  // delete the third row
  EXPECT_EQ( 1, db.exec( "DELETE FROM test WHERE id='3'" ) );
  EXPECT_EQ( 1, db.getChanges() );
  EXPECT_EQ( 3, db.getLastInsertRowid() );
  EXPECT_EQ( 5, db.getTotalChanges() );

  // drop the whole table, ie the two remaining columns
  //  NOTE: here exec() returns 1, like the last, as it is an undefined value for "DROP TABLE" statements
  db.exec( "DROP TABLE IF EXISTS test" );
  EXPECT_FALSE( db.tableExists( "test" ) );
  EXPECT_EQ( 5, db.getTotalChanges() );

  // re-create the same table
  EXPECT_EQ( 1, db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" ) );
  EXPECT_EQ( 5, db.getTotalChanges() );

  // insert two rows wtih two "different" statements => return only 1, ie. for the second INSERT statement
  EXPECT_EQ( 1, db.exec( "INSERT INTO test VALUES (NULL, \"first\");INSERT INTO test VALUES(NULL,\"second\");" ) );
  EXPECT_EQ( 1, db.getChanges() );
  EXPECT_EQ( 2, db.getLastInsertRowid() );
  EXPECT_EQ( 7, db.getTotalChanges() );

#if ( SQLITE_VERSION_NUMBER >= 3007011 )
  // insert two rows with only one statement (starting with SQLite 3.7.11) ==> returns 2
  EXPECT_EQ( 2, db.exec( "INSERT INTO test VALUES (NULL, \"third\"), (NULL,\"fourth\");" ) );
  EXPECT_EQ( 2, db.getChanges() );
  EXPECT_EQ( 4, db.getLastInsertRowid() );
  EXPECT_EQ( 9, db.getTotalChanges() );
#endif
}

TEST( Database, tryExec ) {
  // Create a new database
  SQLite::Database db( ":memory:", SQLite::OPEN_READWRITE );

  // Create a new table with an explicit "id" column aliasing the underlyin rowid
  EXPECT_EQ( SQLite::OK, db.tryExec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" ) );
  EXPECT_EQ( 0, db.getChanges() );
  EXPECT_EQ( 0, db.getLastInsertRowid() );
  EXPECT_EQ( 0, db.getTotalChanges() );

  // first row : insert the "first" text value into new row of id 1
  EXPECT_EQ( SQLite::OK, db.tryExec( "INSERT INTO test VALUES (NULL, \"first\")" ) );
  EXPECT_EQ( 1, db.getChanges() );
  EXPECT_EQ( 1, db.getLastInsertRowid() );
  EXPECT_EQ( 1, db.getTotalChanges() );

  // second row : insert the "second" text value into new row of id 2
  EXPECT_EQ( SQLite::OK, db.tryExec( "INSERT INTO test VALUES (NULL, \"second\")" ) );
  EXPECT_EQ( 1, db.getChanges() );
  EXPECT_EQ( 2, db.getLastInsertRowid() );
  EXPECT_EQ( 2, db.getTotalChanges() );

  // thirs row : insert the "third" text value into new row of id 3
  const std::string insert( "INSERT INTO test VALUES (NULL, \"third\")" );
  EXPECT_EQ( SQLite::OK, db.tryExec( insert ) );
  EXPECT_EQ( 1, db.getChanges() );
  EXPECT_EQ( 3, db.getLastInsertRowid() );
  EXPECT_EQ( 3, db.getTotalChanges() );

  // update second the row : update text to "second_updated"
  EXPECT_EQ( SQLite::OK, db.tryExec( "UPDATE test SET value=\"second-update\" WHERE id='2'" ) );
  EXPECT_EQ( 1, db.getChanges() );
  EXPECT_EQ( 3, db.getLastInsertRowid() );  // last inserted row ID is still 3
  EXPECT_EQ( 4, db.getTotalChanges() );

  // delete the third row
  EXPECT_EQ( SQLite::OK, db.tryExec( "DELETE FROM test WHERE id='3'" ) );
  EXPECT_EQ( 1, db.getChanges() );
  EXPECT_EQ( 3, db.getLastInsertRowid() );
  EXPECT_EQ( 5, db.getTotalChanges() );

  // drop the whole table, ie the two remaining columns
  db.exec( "DROP TABLE IF EXISTS test" );
  EXPECT_FALSE( db.tableExists( "test" ) );
  EXPECT_EQ( 5, db.getTotalChanges() );

  // Re-create the same table
  EXPECT_EQ( SQLite::OK, db.tryExec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" ) );
  EXPECT_EQ( 5, db.getTotalChanges() );

  // insert two rows with two different statements => only 1 change, ie. for the second insert statement
  EXPECT_EQ( 1, db.exec( "INSERT INTO test VALUES (NULL, \"first\");INSERT INTO test VALUES(NULL,\"second\");" ) );
  EXPECT_EQ( 1, db.getChanges() );
  EXPECT_EQ( 2, db.getLastInsertRowid() );
  EXPECT_EQ( 7, db.getTotalChanges() );

#if ( SQLITE_VERSION_NUMBER >= 3007011 )
  // insert two rows with only one statement (starting with SQLite 3.7.11) ==> returns 2
  EXPECT_EQ( 2, db.exec( "INSERT INTO test VALUES (NULL, \"third\"), (NULL,\"fourth\");" ) );
  EXPECT_EQ( 2, db.getChanges() );
  EXPECT_EQ( 4, db.getLastInsertRowid() );
  EXPECT_EQ( 9, db.getTotalChanges() );
#endif
}

TEST( Database, execAndGetTest ) {
  SQLite::Database db( ":memory:", SQLite::OPEN_READWRITE );

  // Create a new table with an explicit "id" column aliasing the underliyng rowid
  db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT, weight INTEGER)" );

  // insert a few rows
  EXPECT_EQ( 1, db.exec( "INSERT INTO test VALUES (NULL, \"first\", 3)" ) );
  EXPECT_EQ( 1, db.exec( "INSERT INTO test VALUES (NULL, \"second\", 5)" ) );
  EXPECT_EQ( 1, db.exec( "INSERT INTO test VALUES (NULL, \"third\", 7)" ) );

  // Get a single value result with an easy to use shortcut
  EXPECT_STREQ( "second", db.execAndGet( "SELECT value FROM test WHERE id=2" ) );
  EXPECT_STREQ( "third", db.execAndGet( "SELECT value FROM test WHERE weight=7" ) );
  const std::string query( "SELECT weight FROM test WHERE value=\"first\"" );
  EXPECT_EQ( 3, db.execAndGet( query ).getInt() );
}

TEST( Database, execException ) {
  // Create a new database
  SQLite::Database db { ":memory:", SQLite::OPEN_READWRITE };
  EXPECT_EQ( SQLite::OK, db.getErrorCode() );
  EXPECT_EQ( SQLite::OK, db.getExtendedErrorCode() );

  // exception with SQLError: "no such table"
  EXPECT_THROW( db.exec( "INSERT INTO test VALUES (NULL, \"first\", 3)" ), SQLite::Exception );
  EXPECT_EQ( SQLITE_ERROR, db.getErrorCode() );
  EXPECT_EQ( SQLITE_ERROR, db.getExtendedErrorCode() );
  EXPECT_STREQ( "no such table: test", db.getErrorMsg() );

  // CREATE a new table
  db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT, weight INTEGER)" );
  EXPECT_EQ( SQLite::OK, db.getErrorCode() );
  EXPECT_EQ( SQLite::OK, db.getExtendedErrorCode() );
  EXPECT_STREQ( "not an error", db.getErrorMsg() );

  // exception with SQL error: Table tst has 3 columns but 2 values were suplied"
  EXPECT_THROW( db.exec( "INSERT INTO test VALUES (NULL, 3)" ), SQLite::Exception );
  EXPECT_EQ( SQLITE_ERROR, db.getErrorCode() );
  EXPECT_EQ( SQLITE_ERROR, db.getExtendedErrorCode() );
  EXPECT_STREQ( "table test has 3 columns but 2 values were supplied", db.getErrorMsg() );

  // exception with SQL error: "No row to get a column from"
  EXPECT_THROW( db.execAndGet( "SELECT weight FROM test WHERE value=\"first\"" ), SQLite::Exception );

  EXPECT_EQ( 1, db.exec( "INSERT INTO test VALUES (NULL, \"first\", 3)" ) );
  // exception with SQL error: "No row to get a column from"
  EXPECT_THROW( db.execAndGet( "SELECT weight FROM test WHERE value=\"second\"" ), SQLite::Exception );

  // Add a row with more values than columns in the table: "table test has 3 columns but 4 values were supplied"
  EXPECT_THROW( db.exec( "INSERT INTO test VALUES (NULL, \"first\", 123, 0.123)" ), SQLite::Exception );
  EXPECT_EQ( SQLITE_ERROR, db.getErrorCode() );
  EXPECT_EQ( SQLITE_ERROR, db.getExtendedErrorCode() );
  EXPECT_STREQ( "table test has 3 columns but 4 values were supplied", db.getErrorMsg() );
}

TEST( Database, tryExecErrorTest ) {
  // Create a new database
  SQLite::Database db { ":memory:", SQLite::OPEN_READWRITE };
  EXPECT_EQ( SQLite::OK, db.getErrorCode() );
  EXPECT_EQ( SQLite::OK, db.getExtendedErrorCode() );

  // insert into nonexistan table : no such table
  EXPECT_EQ( SQLITE_ERROR, db.tryExec( "INSERT INTO test VALUES (NULL, \"first\", 3)" ) );
  EXPECT_EQ( SQLITE_ERROR, db.getErrorCode() );
  EXPECT_EQ( SQLITE_ERROR, db.getExtendedErrorCode() );
  EXPECT_STREQ( "no such table: test", db.getErrorMsg() );

  // CREATE a new table
  EXPECT_EQ( SQLite::OK, db.tryExec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT, weight INTEGER)" ) );
  EXPECT_EQ( SQLite::OK, db.getErrorCode() );
  EXPECT_EQ( SQLite::OK, db.getExtendedErrorCode() );
  EXPECT_STREQ( "not an error", db.getErrorMsg() );

  // Add a row with fewer values than columns in the table: "table test has 3 columns but 2 values were supplied"
  EXPECT_EQ( SQLITE_ERROR, db.tryExec( "INSERT INTO test VALUES (NULL, 3)" ) );
  EXPECT_EQ( SQLITE_ERROR, db.getErrorCode() );
  EXPECT_EQ( SQLITE_ERROR, db.getExtendedErrorCode() );
  EXPECT_STREQ( "table test has 3 columns but 2 values were supplied", db.getErrorMsg() );

  // Add a row with more values than columns in the table: "table test has 3 columns but 4 values were supplied"
  EXPECT_EQ( SQLITE_ERROR, db.tryExec( "INSERT INTO test VALUES (NULL, \"first\", 123, 0.123)" ) );
  EXPECT_EQ( SQLITE_ERROR, db.getErrorCode() );
  EXPECT_EQ( SQLITE_ERROR, db.getExtendedErrorCode() );
  EXPECT_STREQ( "table test has 3 columns but 4 values were supplied", db.getErrorMsg() );

  // Create first row
  EXPECT_EQ( SQLite::OK, db.tryExec( "INSERT INTO test VALUES (1, \"first\", 3)" ) );
  EXPECT_EQ( 1, db.getLastInsertRowid() );

  // try to insert a new row with the same PRIMARY KEY: "UNIQUE constraint failed: test.id"
  EXPECT_EQ( SQLITE_CONSTRAINT, db.tryExec( "INSERT INTO test VALUES (1, \"impossible\", 456)" ) );
  EXPECT_EQ( SQLITE_CONSTRAINT, db.getErrorCode() );
  EXPECT_EQ( SQLITE_CONSTRAINT_PRIMARYKEY, db.getExtendedErrorCode() );
  EXPECT_STREQ( "UNIQUE constraint failed: test.id", db.getErrorMsg() );
}

static void firstchar( sqlite3_context* context, int argc, sqlite3_value** argv ) {
  if ( argc == 1 ) {
    const unsigned char* text = sqlite3_value_text( argv[0] );
    if ( text && text[0] ) {
      char result[2];
      result[0] = text[0];
      result[1] = '\0';
      sqlite3_result_text( context, result, -1, SQLITE_TRANSIENT );
      return;
    }
  }
  sqlite3_result_null( context );
}

TEST( Database, createFuncitonTest ) {
  SQLite::Database db { ":memory:", SQLite::OPEN_READWRITE };
  db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" );

  EXPECT_EQ( 1, db.exec( "INSERT INTO test VALUES (NULL, \"first\")" ) );
  EXPECT_EQ( 1, db.exec( "INSERT INTO test VALUES (NULL, \"second\")" ) );

  // exception with SQLError: "no such function: firstchar"
  EXPECT_THROW( db.exec( "SELECT firstchar(value) FROM test WHERE id=1" ), SQLite::Exception );

  db.createFunction( "firstchar", 1, true, nullptr, &firstchar, nullptr, nullptr, nullptr );

  EXPECT_EQ( 1, db.exec( "SELECT firstchar(value) FROM test WHERE id=1" ) );
}

TEST( Database, loadExtensionTest ) {
  SQLite::Database db { ":memory:", SQLite::OPEN_READWRITE };

  // try to load a non-existing extension (no dynamic library found)
  EXPECT_THROW( db.loadExtension( "non-existing-extension", "entry-point" ), SQLite::Exception );

  // TODO: test a proper extension
}

TEST( Database, getHeaderInfoTest ) {
  remove( "test.db3" );
  {
    // Call without passing a database file name
    EXPECT_THROW( SQLite::Database::getHeaderInfo( "" ), SQLite::Exception );

    // Call with a non-existent database
    EXPECT_THROW( SQLite::Database::getHeaderInfo( "test.db3" ), SQLite::Exception );

    // Simulate an incomplete header by writing garbage to file
    {
      const unsigned char badData[] = "garbage...";
      const char* pBadData = reinterpret_cast<const char*>( &badData[0] );

      remove( "short.db3" );
      std::ofstream corruptDb;
      corruptDb.open( "short.db3", std::ios::app | std::ios::binary );
      corruptDb.write( pBadData, sizeof( badData ) );
      corruptDb.close();

      EXPECT_THROW( SQLite::Database::getHeaderInfo( "short.db3" ), SQLite::Exception );
      remove( "short.db3" );
    }

    // Simulate a corrupt header by writing garbage to file
    {
      const unsigned char badData[100] = "garbage...";
      const char* pBadData = reinterpret_cast<const char*>( &badData[0] );

      remove( "corrupt.db3" );
      std::ofstream corruptDb;
      corruptDb.open( "corrupt.db3", std::ios::app | std::ios::binary );
      corruptDb.write( pBadData, sizeof( badData ) );
      corruptDb.close();

      EXPECT_THROW( SQLite::Database::getHeaderInfo( "corrupt.db3" ), SQLite::Exception );
      remove( "corrupt.db3" );
    }

    // Create a new database
    SQLite::Database db { "test.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE };
    db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" );

    // Set assorted SQLite header values using associated PRAGMA
    db.exec( "PRAGMA main.user_version = 12345" );
    db.exec( "PRAGMA main.application_id = 2468" );

    // Parse header fields from test database
    const SQLite::Header h = db.getHeaderInfo();

    // Test header values explicilty set via PRAGMA statements
    EXPECT_EQ( h.userVersion, 12345 );
    EXPECT_EQ( h.applicationId, 2468 );

    // Test header values with expected default values
    EXPECT_EQ( h.pageSizeBytes, 4096 );
    EXPECT_EQ( h.fileFormatWriteVersion, 1 );
    EXPECT_EQ( h.fileFormatReadVersion, 1 );
    EXPECT_EQ( h.reservedSpaceBytes, 0 );
    EXPECT_EQ( h.maxEmbeddedPayloadFrac, 64 );
    EXPECT_EQ( h.minEmbeddedPayloadFrac, 32 );
    EXPECT_EQ( h.leafPayloadFrac, 32 );
    EXPECT_EQ( h.fileChangeCounter, 3 );
    EXPECT_EQ( h.databaseSizePages, 2 );
    EXPECT_EQ( h.firstFreelistTrunkPage, 0 );
    EXPECT_EQ( h.totalFreelistPages, 0 );
    EXPECT_EQ( h.schemaCookie, 1 );
    EXPECT_EQ( h.schemaFormatNumber, 4 );
    EXPECT_EQ( h.defaultPageCacheSizeBytes, 0 );
    EXPECT_EQ( h.largestBTreePageNumber, 0 );
    EXPECT_EQ( h.databaseTextEncoding, 1 );
    EXPECT_EQ( h.incrementalVaccumMode, 0 );
    EXPECT_EQ( h.versionValidFor, 3 );
    EXPECT_EQ( h.sqliteVersion, SQLITE_VERSION_NUMBER );
  }
  remove( "test.db3" );
}

#ifdef SQLITE_HAS_CODEC
TEST( Database, encrypAndDecryptTest ) {
  remove( "test.db3" );
  {
    // Try to open the non-existing database
    EXPECT_THROW( SQLite::Database not_found( "test.db3" ), SQLite::Exception );
    EXPECT_THROW( SQLite::Database::isUnencrypted( "test.db3" ), SQLite::Exception );
    EXPECT_THROW( SQLite::Database::isUnencrypted( "" ), SQLite::Exception );

    // Create a new database
    SQLite::Database db { "test.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE };
    EXPECT_FALSE( db.tableExists( "test" ) );
    db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" );
    EXPECT_TRUE( db.tableExists( "test" ) );
  }  // close DB test.db3
  {
    // Reopen the database file and encrypt it
    EXPECT_TRUE( SQLite::Database::isUnencrypted( "test.db3" ) );
    SQLite::Database db( "test.db3", SQLite::OPEN_READWRITE );
    //  Encrypt the database
    db.rekey( "123secret" );
  }  // close DB test.db3
  {
    // Reopen the database file and try to use it
    EXPECT_FALSE( SQLite::Database::isUnencrypted( "test.db3" ) );
    SQLite::Database db( "test.db3", SQLite::OPEN_READONLY );
    EXPECT_THROW( db.tableExists( "test" ), SQLite::Exception );
    db.key( "123secret" );
    EXPECT_TRUE( db.tableExists( "test" ) );
  }  // close DB test.db3
  {
    // Reopen the database file and Decrypt it
    EXPECT_FALSE( SQLite::Database::isUnencrypted( "test.db3" ) );
    SQLite::Database db( "test.db3", SQLite::OPEN_READWRITE );
    //  Decrypt the database
    db.key( "123secret" );
    db.rekey( "" );
  }  // close DB test.db3
  {
    // Reopen the database file and use it
    EXPECT_TRUE( SQLite::Database::isUnencrypted( "test.db3" ) );
    SQLite::Database db( "test.db3", SQLite::OPEN_READWRITE );
    EXPECT_TRUE( db.tableExists( "test" ) );
  }  // close DB test.db3
}

#else  // SQLITE_HAS_CODEC
TEST( Database, encrypAndDecryptTest ) {
  remove( "test.db3" );
  {
    // Try to open the non-existing database
    EXPECT_THROW( SQLite::Database not_found( "test.db3" ), SQLite::Exception );
    EXPECT_THROW( SQLite::Database::isUnencrypted( "test.db3" ), SQLite::Exception );
    EXPECT_THROW( SQLite::Database::isUnencrypted( "" ), SQLite::Exception );

    // Create a new database
    SQLite::Database db { "test.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE };
    EXPECT_FALSE( db.tableExists( "test" ) );
    db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" );
    EXPECT_TRUE( db.tableExists( "test" ) );
  }  // close DB test.db3
  {
    // Reopen the database file and encrypt it
    EXPECT_FALSE( SQLite::Database::isUnencrypted( "test.db3" ) );
    SQLite::Database db( "test.db3", SQLite::OPEN_READWRITE );
    // Encrypt the database
    EXPECT_THROW( db.key( "123secret" ), SQLite::Exception );
    EXPECT_THROW( db.rekey( "123secret" ), SQLite::Exception );
  }  // close DB test.db3
  remove( "test.db3" );
}
#endif