#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <filesystem>

#include "SQLiteCpp.h"
#include "VariadicBind.h"

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

static inline std::string getExamplePath() {
  return std::filesystem::current_path().parent_path().string() + "/" + "examples/example1";
}

// Example database
static const std::string filename_example_db3 = getExamplePath() + "/" + "example.db3";
/// Image (SQLite logo as a 12581 bytes PNG file)
static const int sizeof_logo_png = 12581;
static const std::string filename_logo_png = getExamplePath() + "/" + "logo.png";

class Example {
  public:
    Example() = default;
    virtual ~Example() {};

    // List the rows where the "weight" column is greater than the provided aParamValue
    void ListGreaterThan( const int aParamValue ) {
      std::cout << "ListGreaterThan(" << aParamValue << ")\n";

      // Bind the integer value provided to the first parameter of the SQL query
      m_Query.bind( ":min_weight", aParamValue );  // same as m_Query.bind(1, aParamValue);

      // Loop to execute the query step by step, to get one a row of results at a time
      while ( m_Query.executeStep() ) {
        std::cout << "row (" << m_Query.getColumn( 0 ) << ", \"" << m_Query.getColumn( 1 ) << "\" , "
                  << m_Query.getColumn( 2 ) << ")\n";
      }

      m_Query.reset();
    }

  private:
    SQLite::Database m_Db { filename_example_db3 };
    SQLite::Statement m_Query { m_Db, "SELECT * FROM test WHERE weight > :min_weight" };
};

int main() {
  // Using SQLITE_VERSION would require #include <sqlite3.h> which we want to avoid: use SQLite::VERSION if
  // possible. std::cout << "SQlite3 version " << SQLITE_VERSION << std::endl;
  std::cout << "SQlite3 version " << SQLite::VERSION << " (" << SQLite::getLibVersion() << ")" << std::endl;
  std::cout << "SQliteC++ version " << SQLITECPP_VERSION << std::endl;

  ////////////////////////////////////////////////////////////////////////////
  // Inspect a database via SQLite header information
  try {
    const SQLite::Header header = SQLite::Database::getHeaderInfo( filename_example_db3 );

    // Print values for all header fields
    // Official documentation for fields can be found here: https://www.sqlite.org/fileformat.html#the_database_header
    std::cout << "Magic header string: " << header.headerStr << "\n";
    std::cout << "Page size bytes: " << header.pageSizeBytes << "\n";
    std::cout << "File format write version: " << (int)header.fileFormatWriteVersion << "\n";
    std::cout << "File format read version: " << (int)header.fileFormatReadVersion << "\n";
    std::cout << "Reserved space bytes: " << (int)header.reservedSpaceBytes << "\n";
    std::cout << "Max embedded payload fraction " << (int)header.maxEmbeddedPayloadFrac << "\n";
    std::cout << "Min embedded payload fraction: " << (int)header.minEmbeddedPayloadFrac << "\n";
    std::cout << "Leaf payload fraction: " << (int)header.leafPayloadFrac << "\n";
    std::cout << "File change counter: " << header.fileChangeCounter << "\n";
    std::cout << "Database size pages: " << header.databaseSizePages << "\n";
    std::cout << "First freelist trunk page: " << header.firstFreelistTrunkPage << "\n";
    std::cout << "Total freelist trunk pages: " << header.totalFreelistPages << "\n";
    std::cout << "Schema cookie: " << header.schemaCookie << "\n";
    std::cout << "Schema format number: " << header.schemaFormatNumber << "\n";
    std::cout << "Default page cache size bytes: " << header.defaultPageCacheSizeBytes << "\n";
    std::cout << "Largest B tree page number: " << header.largestBTreePageNumber << "\n";
    std::cout << "Database text encoding: " << header.databaseTextEncoding << "\n";
    std::cout << "User version: " << header.userVersion << "\n";
    std::cout << "Incremental vaccum mode: " << header.incrementalVaccumMode << "\n";
    std::cout << "Application ID: " << header.applicationId << "\n";
    std::cout << "Version valid for: " << header.versionValidFor << "\n";
    std::cout << "SQLite version: " << header.sqliteVersion << "\n";
  } catch ( const std::exception& e ) {
    std::cout << "SQLite exception: " << e.what() << "\n";
    return EXIT_FAILURE;  // unexpected error : exit the example program
  }

  // Very basic first example (1/7) :
  try {
    // Open a database file in read-only mode
    SQLite::Database db { filename_example_db3 };
    std::cout << "SQLite database file '" << db.getFilename().c_str() << "' open successfully\n";

    // Test if the 'test' table exists
    const bool bExists = db.tableExists( "test" );
    std::cout << "SQLite table 'test' exists=" << bExists << "\n";

    // Get a single value result with an easy to use shortcut
    const std::string value = db.execAndGet( "SELECT value VALUE from test WHERE id=2" );
    std::cout << "execAndGet=" << value << "\n";
  } catch ( const std::exception& e ) {
    std::cout << "SQLite exception: " << e.what() << "\n";
    return EXIT_FAILURE;  // unexpected error : exit the example program
  }

  // simple select query - few variations (2/7) :
  try {
    // Open a database file in read-only mode
    SQLite::Database db { filename_example_db3 };
    std::cout << "SQLite database file '" << db.getFilename().c_str() << "' open successfully\n";

    // compile a SQL query, containing one parameter (index 1)
    SQLite::Statement query {
      db, "SELECT id as test_id, value as test_val, weight as test_weight FROM test WHERE weight > ?"
    };

    std::cout << "SQLite statement '" << query.getQuery().c_str() << "' compiled(" << query.getColumnCount()
              << " columns in the result)\n";

    // Bind the integer value 2 to the first parameter of the SQL query
    query.bind( 1, 2 );
    std::cout << "binded with integer value '2' : \n";

    // loop to execute the query step by step, to get one a row of results at a time
    while ( query.executeStep() ) {
      const int id = query.getColumn( 0 );
      const std::string value = query.getColumn( 1 );
      const int bytes = query.getColumn( 1 ).size();
      const double weight = query.getColumn( 2 );

      std::cout << "row (" << id << ", \"" << value.c_str() << "\"(" << bytes << ") " << weight << ")\n";
    }

    // Reset the query to use it again
    query.reset();
    std::cout << "SQLite statement '" << query.getQuery().c_str() << "' reseted (" << query.getColumnCount()
              << " columns in the result)\n";

    // Show how to get the aliased names of the result columns.
    const std::string name0 = query.getColumnName( 0 );
    const std::string name1 = query.getColumnName( 1 );
    const std::string name2 = query.getColumnName( 2 );
    std::cout << "aliased result [\"" << name0.c_str() << "\", \"" << name1.c_str() << "\", \"" << name2.c_str()
              << "\"]\n";

#ifdef SQLITE_ENABLE_COLUMN_METADATA
    // Show how to get origin names of the table columns from which theses result columns come from.
    // Requires the SQLITE_ENABLE_COLUMN_METADATA preprocessor macro to be
    // also defined at compile times of the SQLite library itself.
    const std::string oname0 = query.getColumnOriginName( 0 );
    const std::string oname1 = query.getColumnOriginName( 1 );
    const std::string oname2 = query.getColumnOriginName( 2 );
    std::cout << "origin table 'test' [\"" << oname0.c_str() << "\", \"" << oname1.c_str() << "\", \"" << oname2.c_str()
              << "\"]\n";
#endif

    while ( query.executeStep() ) {
      // demonstrates that inserting column value in a std::ostream is natural
      std::cout << "row (" << query.getColumn( 0 ) << ", \"" << query.getColumn( 1 ) << "\", " << query.getColumn( 2 )
                << ")\n";
    }

    // get columns by name
    query.reset();
    std::cout << "SQLite statement '" << query.getQuery().c_str() << "' reseted (" << query.getColumnCount()
              << " columns in the result)\n";

    while ( query.executeStep() ) {
      const int id = query.getColumn( "test_id" );
      const std::string value = query.getColumn( "test_val" );
      const double weight = query.getColumn( "test_weight" );
      std::cout << "row (" << id << ", \"" << value.c_str() << "\"" << weight << ")\n";
    }

    // uses explicit typed getters instead of auto cast operators
    query.reset();
    std::cout << "SQLite statement '" << query.getQuery().c_str() << "' reseted (" << query.getColumnCount()
              << " columns in the result)\n";

    // Bind the string value "6" to the first parameter of the SQL query
    query.bind( 1, "6" );
    //    reuses variables: uses alignment operator in the loop instead of constructor with initialization
    int id = 0;
    std::string value;
    double weight = 0.0;
    while ( query.executeStep() ) {
      id = query.getColumn( 0 ).getInt();
      value = query.getColumn( 1 ).getText();
      weight = query.getColumn( 2 ).getInt();
      std::cout << "row (" << id << ", \"" << value << "\", " << weight << ")\n";
    }

  } catch ( const std::exception& e ) {
    std::cout << "SQLite exception: " << e.what() << "\n";
    return EXIT_FAILURE;  // unexpected error : exit the example program
  }

  try {
    // Object oriented Basic example (3/7)
    Example example;

    example.ListGreaterThan( 8 );
    example.ListGreaterThan( 6 );
    example.ListGreaterThan( 2 );

  } catch ( const std::exception& e ) {
    std::cout << "SQLite exception: " << e.what() << "\n";
    return EXIT_FAILURE;  // unexpected error : exit the example program
  }

  // the execAndGet wrapper example(4/7)
  try {
    SQLite::Database db( filename_example_db3 );
    std::cout << "SQLite database file '" << db.getFilename().c_str() << "'opened successfully\n";

    //!!!WARNING : be very careful wtih this dangerous method: you have to
    // make a COPY OF THE result, else it will be destroy before the next line
    // (when underlying temporary Statement and Column objects are destroyed)
    std::string value = db.execAndGet( "SELECT value FROM test WHERE id=2" );
    std::cout << "execAndGet=" << value.c_str() << "\n";

  } catch ( const std::exception& e ) {
    std::cout << "SQLite exception: " << e.what() << "\n";
    return EXIT_FAILURE;  // unexpected error : exit the example program
  }

  // Simple batch quaries example(5/7)
  try {
    SQLite::Database db( "test.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE );
    std::cout << "SQLite database file '" << db.getFilename().c_str() << "' opened successfully\n";

    // Create a new table with an explicit "id" column aliasing the underlying rowid
    db.exec( "DROP TABLE IF EXISTS test" );
    db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" );

    // first row
    int nb = db.exec( "INSERT INTO test VALUES (NULL, \"test\")" );
    std::cout << "INSERT INTO test VALUES (NULL, \"test\")\", returned " << nb << '\n';

    // second row
    nb = db.exec( "INSERT INTO test VALUES (NULL, \"second\")" );
    std::cout << "INSERT INTO test VALUES (NULL, \"second\")\", returned " << nb << '\n';

    // update second row
    nb = db.exec( "UPDATE test SET value=\"second-updated\" WHERE id='2'" );
    std::cout << "UPDATE test SET value=\"second-updated\" WHERE id='2', returned " << nb << '\n';

    nb = db.getTotalChanges();
    std::cout << "nb of total changes since connection: " << nb << "\n";

    // Check the results: expect two row of result
    SQLite::Statement query { db, "SELECT * FROM test" };
    std::cout << "SELECT * FROM test"
              << "\n";
    while ( query.executeStep() ) {
      std::cout << "row (" << query.getColumn( 0 ) << ", \"" << query.getColumn( 1 ) << "\")\n";
    }
    db.exec( "DROP TABLE test" );

  } catch ( const std::exception& e ) {
    std::cout << "SQLite exception: " << e.what() << "\n";
    return EXIT_FAILURE;  // unexpected error : exit the example program
  }

  remove( "test.db3" );

  // RAII transaction example (6/7)
  try {
    SQLite::Database db { "transaction.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE };
    std::cout << "SQLite database file '" << db.getFilename().c_str() << "' opened successfully\n";

    db.exec( "DROP TABLE IF EXISTS test" );

    // Example of a successfull transaction:
    try {
      // Begin transaction
      SQLite::Transaction transaction { db };
      db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" );

      // first row
      int nb = db.exec( "INSERT INTO test VALUES (NULL, \"test\")" );
      std::cout << "INSERT INTO test VALUES (NULL, \"test\")\", returned " << nb << '\n';

      // commit transaction
      transaction.commit();

    } catch ( const std::exception& e ) {
      std::cout << "SQLite exception: " << e.what() << "\n";
      return EXIT_FAILURE;  // unexpected error : exit the example program
    }

    // Exemple of a rollbacked transaction :
    try {
      // Begin transaction
      SQLite::Transaction transaction( db );

      int nb = db.exec( "INSERT INTO test VALUES (NULL, \"second\")" );
      std::cout << "INSERT INTO test VALUES (NULL, \"second\")\", returned " << nb << std::endl;

      nb = db.exec( "INSERT INTO test ObviousError" );
      std::cout << "INSERT INTO test \"error\", returned " << nb << std::endl;

      return EXIT_FAILURE;  // we should never get there : exit the example program

      // Commit transaction
      transaction.commit();
    } catch ( std::exception& e ) {
      std::cout << "SQLite exception: " << e.what() << std::endl;
      // expected error, see above
    }

    // Check the results (expect only one row of result, as the second one has been rollbacked by the error)
    SQLite::Statement query( db, "SELECT * FROM test" );
    std::cout << "SELECT * FROM test"
              << "\n";

    while ( query.executeStep() ) {
      std::cout << "row (" << query.getColumn( 0 ) << ", \"" << query.getColumn( 1 ) << "\")\n";
    }

  } catch ( const std::exception& e ) {
    std::cout << "SQLite exception: " << e.what() << "\n";
    return EXIT_FAILURE;  // unexpected error : exit the example program
  }
  remove( "transaction.db3" );

  // Binary blob and in-memory database example (7/7) :
  try {
    // Open a database file in create/write mode
    SQLite::Database db( ":memory:", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE );
    std::cout << "SQLite database file '" << db.getFilename().c_str() << "' opened successfully\n";

    db.exec( "DROP TABLE IF EXISTS test" );
    db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value BLOB)" );

    // A) insert the logo.png image into the db as a blob
    FILE* fp = fopen( filename_logo_png.c_str(), "rb" );
    if ( NULL != fp ) {
      char buffer[16 * 1024];
      static_assert( sizeof( buffer ) > sizeof_logo_png, "Buffer is smaller than the size of the file to read" );
      void* blob = &buffer;
      const int size = static_cast<int>( fread( blob, 1, 16 * 1024, fp ) );
      buffer[size] = '\0';
      SQLITECPP_ASSERT( size == sizeof_logo_png,
                        "unexpected fread return value" );  // See SQLITECPP_ENABLE_ASSERT_HANDLER
      fclose( fp );
      std::cout << filename_logo_png << " file size=" << size << " bytes\n";

      // Insert query
      SQLite::Statement query( db, "INSERT INTO test VALUES (NULL, ?)" );
      // Bind the blob value to the first parameter of the SQL query
      query.bind( 1, blob, size );
      std::cout << "blob binded successfully\n";

      // Execute the one-step query to insert the blob
      int nb = query.exec();
      std::cout << "INSERT INTO test VALUES (NULL, ?)\", returned " << nb << std::endl;
    } else {
      std::cout << "file " << filename_logo_png << " not found !\n";
      return EXIT_FAILURE;  // unexpected error : exit the example program
    }

    // B) select the blob from the db and write it to disk into a "out.png" image file
    fp = fopen( "out.png", "wb" );
    if ( NULL != fp ) {
      SQLite::Statement query( db, "SELECT * FROM test" );
      std::cout << "SELECT * FROM test :\n";
      if ( query.executeStep() ) {
        SQLite::Column colBlob = query.getColumn( 1 );
        const void* const blob = colBlob.getBlob();
        const size_t size = colBlob.getBytes();
        std::cout << "row (" << query.getColumn( 0 ) << ", size=" << size << " bytes)\n";
        size_t sizew = fwrite( blob, 1, size, fp );
        SQLITECPP_ASSERT( sizew == size, "fwrite failed" );  // See SQLITECPP_ENABLE_ASSERT_HANDLER
        fclose( fp );
      }
      // NOTE: here the blob is still held in memory, until the Statement is finalized at the end of the scope
    } else {
      std::cout << "file out.png not created !\n";
      return EXIT_FAILURE;  // unexpected error : exit the example program
    }
  } catch ( std::exception& e ) {
    std::cout << "SQLite exception: " << e.what() << std::endl;
    return EXIT_FAILURE;  // unexpected error : exit the example program
  }
  remove( "out.png" );

  // example with C++14 variadic bind
  try {
    // Open a database file in create/write mode
    SQLite::Database db( ":memory:", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE );

    db.exec( "DROP TABLE IF EXISTS test" );
    db.exec( "CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)" );

    {
      SQLite::Statement query( db, "INSERT INTO test VALUES (?, ?)" );

      SQLite::bind( query, 42, "fortytwo" );
      // Execute the one-step query to insert the blob
      int nb = query.exec();
      std::cout << "INSERT INTO test VALUES (NULL, ?)\", returned " << nb << std::endl;
    }

    SQLite::Statement query( db, "SELECT * FROM test" );
    std::cout << "SELECT * FROM test :\n";
    if ( query.executeStep() ) {
      std::cout << query.getColumn( 0 ).getInt() << "\t\"" << query.getColumn( 1 ).getText() << "\"\n";
    }
  } catch ( std::exception& e ) {
    std::cout << "SQLite exception: " << e.what() << std::endl;
    return EXIT_FAILURE;  // unexpected error : exit the example program
  }

  std::cout << "everything ok, quitting\n";
  return 0;
}
