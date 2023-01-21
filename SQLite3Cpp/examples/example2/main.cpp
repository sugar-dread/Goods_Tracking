/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief A few short examples in a row. Demonstrates how-to use SQLiteCpp wrapper
 * @version 0.1
 * @date 2022-11-30
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "SQLiteCpp.h"

#ifdef SQLITECPP_ENABLE_ASSERT_HANDLER
namespace SQLite {
// definition of the assertion handler enabled with SQLITECPP_ENABLE_ASSERT HANDLER is defined in the project
void assertion_failed( const char* apFile, const long apLine, const char* apFunc, const char* apExpr,
                       const char* apMsg ) {
  // Print a message to the standrd error output stream, and abort the program
  std::cerr << apFile << ":" << apLine << ":"
            << " error : assertion failed (" << apExpr << ") in " << apFunc << "() with message \"" << apMsg << "\"\n";
  std::abort();
}

}  // namespace SQLite

#endif

int main() {
  // Using SQLITE_VERSION would require #include<sqlite3.h> which we want to avoid: use SQLite::VERSION if possible
  std::cout << "SQLite3 version " << SQLite::VERSION << " (" << SQLite::getLibVersion() << ")\n ";
  std::cout << "SQLite3C++ version " << SQLITECPP_VERSION << "\n";

  // Simple batch queries example:
  try {
    // Open a database file in create/write mode
    SQLite::Database db { "test.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE };
    std::cout << "SQLite database file " << db.getFilename().c_str() << " opened successfully\n";

    // Create a new table with an explicit "id" column aliasing the underlying rowid
    db.exec( "DROP TABLE IF EXISTS test" );
    db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" );

    // first row
    int nb = db.exec( "INSERT INTO test VALUES (NULL, \"test\")" );
    std::cout << "INSERT INTO test VALUES (NULL, \"test\")\", returned " << nb << "\n";

    // second row
    nb = db.exec( "INSERT INTO test VALUES (NULL, \"second\")" );
    std::cout << "INSERT INTO test VALUES (NULL, \"second\")\", returned " << nb << "\n";

    // update the second row
    nb = db.exec( "UPDATE test SET value=\"second-updated\" WHERE id='2'" );
    std::cout << "UPDATE test SET value=\"second-updated\" WHERE id='2', return " << nb << "\n";

    // Check the results: expect two of result
    SQLite::Statement query( db, "SELECT * FROM test" );
    std::cout << "SELECT * FROM test : \n";

    while ( query.executeStep() ) {
      std::cout << "row (" << query.getColumn( 0 ) << ", \"" << query.getColumn( 1 ) << "\")\n";
    }

    db.exec( "DROP TABLE test" );

  } catch ( const std::exception& e ) {
    std::cout << "SQLite exception : " << e.what() << "\n";
    return EXIT_FAILURE;
  }
  remove( "test.db3" );

  return 0;
}