/**
 * @file    VariadicBind_test.cpp
 * @ingroup tests
 * @brief   Test of variadic bind

 */

#include "Database.h"
#include "Statement.h"
#include "ExecuteMany.h"

#include <gtest/gtest.h>

#include <cstdio>

TEST( ExecuteMany, invalid ) {
  // Create a new database
  SQLite::Database db( ":memory:", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE );

  EXPECT_EQ( 0, db.exec( "DROP TABLE IF EXISTS test" ) );
  EXPECT_EQ( 0, db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT DEFAULT 'default')" ) );
  EXPECT_TRUE( db.tableExists( "test" ) );
  { execute_many( db, "INSERT INTO test VALUES (?, ?)", 1, std::make_tuple( 2 ), std::make_tuple( 3, "three" ) ); }
  // make sure the content is as expected
  {
    SQLite::Statement query( db, std::string { "SELECT id, value FROM test ORDER BY id" } );
    std::vector<std::pair<int, std::string> > results;
    while ( query.executeStep() ) {
      const int id = query.getColumn( 0 );
      std::string value = query.getColumn( 1 );
      results.emplace_back( id, std::move( value ) );
    }
    EXPECT_EQ( std::size_t( 3 ), results.size() );

    EXPECT_EQ( std::make_pair( 1, std::string { "" } ), results.at( 0 ) );
    EXPECT_EQ( std::make_pair( 2, std::string { "" } ), results.at( 1 ) );
    EXPECT_EQ( std::make_pair( 3, std::string { "three" } ), results.at( 2 ) );
  }
}
