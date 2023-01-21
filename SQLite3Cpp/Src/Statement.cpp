/**
 * @file
 * Statement.cpp
 * @author your name
 * (you@domain.com)
 * @brief A source
 * code of
 * Statement.h
 * @version 0.1
 * @date 2022-11-24
 *
 * @copyright
 * Copyright (c)
 * 2022
 *
 */

#include "Statement.h"
#include "Assertion.h"
#include "Column.h"
#include "Database.h"

namespace SQLite {

Statement::Statement( const Database& aDatabase, const char* apQuery )
    : m_Query( apQuery ),
      m_pSQLite( aDatabase.getHandle() ),
      m_pPreparedStatement( prepareStatement() )  // prepare the SQL query (needs Database
                                                  // friendship)

{
  m_ColumnCount = sqlite3_column_count( m_pPreparedStatement.get() );
}

Statement::Statement( const Database& aDatabase, const std::string& aQuery ) : Statement( aDatabase, aQuery.c_str() ) {}

Statement::Statement( Statement&& aStatement ) noexcept
    : m_Query( std::move( aStatement.m_Query ) ),
      m_pSQLite( aStatement.m_pSQLite ),
      m_pPreparedStatement( std::move( aStatement.m_pPreparedStatement ) ),
      m_ColumnCount( aStatement.m_ColumnCount ),
      m_HasRow( aStatement.m_HasRow ),
      m_Done( aStatement.m_Done ),
      m_ColumNames( std::move( aStatement.m_ColumNames ) ) {
  aStatement.m_pSQLite = nullptr;
  aStatement.m_ColumnCount = 0;
  aStatement.m_HasRow = false;
  aStatement.m_Done = false;
}

void Statement::reset() {
  const int ret = tryReset();
  check( ret );
}

int Statement::tryReset() noexcept {
  m_HasRow = false;
  m_Done = false;
  return sqlite3_reset( m_pPreparedStatement.get() );
}

void Statement::clearBindings() const {
  const int ret = sqlite3_clear_bindings( getPreparedStatement() );
  check( ret );
}

int Statement::getIndex( const char* const apName ) const {
  return sqlite3_bind_parameter_index( getPreparedStatement(), apName );
}

void Statement::bind( const int aIndex, const int32_t aValue ) const {
  const int ret = sqlite3_bind_int( getPreparedStatement(), aIndex, aValue );
  check( ret );
}

void Statement::bind( const int aIndex, const uint32_t aValue ) const {
  const int ret = sqlite3_bind_int64( getPreparedStatement(), aIndex, aValue );
  check( ret );
}

void Statement::bind( const int aIndex, const int64_t aValue ) const {
  const int ret = sqlite3_bind_int64( getPreparedStatement(), aIndex, aValue );
  check( ret );
}

void Statement::bind( const int aIndex, const double aValue ) const {
  const int ret = sqlite3_bind_double( getPreparedStatement(), aIndex, aValue );
  check( ret );
}

void Statement::bind( const int aIndex, const std::string& aValue ) const {
  const int ret = sqlite3_bind_text( getPreparedStatement(), aIndex, aValue.c_str(), static_cast<int>( aValue.size() ),
                                     SQLITE_TRANSIENT );
  check( ret );
}

void Statement::bind( const int aIndex, const char* aValue ) const {
  const int ret = sqlite3_bind_text( getPreparedStatement(), aIndex, aValue, -1, SQLITE_TRANSIENT );
  check( ret );
}

void Statement::bind( const int aIndex, const void* aValue, const int aSize ) const {
  const int ret = sqlite3_bind_blob( getPreparedStatement(), aIndex, aValue, aSize, SQLITE_TRANSIENT );
  check( ret );
}

void Statement::bindNoCopy( const int aIndex, const char* aValue ) const {
  const int ret = sqlite3_bind_text( getPreparedStatement(), aIndex, aValue, -1, SQLITE_STATIC );
  check( ret );
}

void Statement::bind( const int aIndex ) const {
  const int ret = sqlite3_bind_null( getPreparedStatement(), aIndex );
  check( ret );
}

void Statement::bind( const char* apName, const int32_t aValue ) const { bind( getIndex( apName ), aValue ); }

void Statement::bind( const char* apName, const uint32_t aValue ) const { bind( getIndex( apName ), aValue ); }

void Statement::bind( const char* apName, const int64_t aValue ) const { bind( getIndex( apName ), aValue ); }

void Statement::bind( const char* apName, const double aValue ) const { bind( getIndex( apName ), aValue ); }

void Statement::bind( const char* apName, const std::string& aValue ) const { bind( getIndex( apName ), aValue ); }

void Statement::bind( const char* apName, const char* aValue ) const { bind( getIndex( apName ), aValue ); }

void Statement::bind( const char* apName, const void* aValue, const int aSize ) const {
  bind( getIndex( apName ), aValue, aSize );
}

void Statement::bind( const char* apName ) const { bind( getIndex( apName ) ); }

void Statement::bind( const std::string& aName, const int32_t aValue ) const { bind( aName.c_str(), aValue ); }

void Statement::bind( const std::string& aName, const uint32_t aValue ) const { bind( aName.c_str(), aValue ); }

void Statement::bind( const std::string& aName, const int64_t aValue ) const { bind( aName.c_str(), aValue ); }

void Statement::bind( const std::string& aName, const double aValue ) const { bind( aName.c_str(), aValue ); }

void Statement::bind( const std::string& aName, const std::string& aValue ) const { bind( aName.c_str(), aValue ); }

void Statement::bind( const std::string& aName, const char* aValue ) const { bind( aName.c_str(), aValue ); }

void Statement::bind( const std::string& aName, const void* aValue, const int aSize ) const {
  bind( aName.c_str(), aValue, aSize );
}

void Statement::bind( const std::string& aName ) const { bind( aName.c_str() ); }

void Statement::bindNoCopy( const int aIndex, const std::string& aValue ) const {
  const int ret = sqlite3_bind_text( getPreparedStatement(), aIndex, aValue.c_str(), static_cast<int>( aValue.size() ),
                                     SQLITE_STATIC );
  check( ret );
}

void Statement::bindNoCopy( const int aIndex, const void* aValue, const int aSize ) const {
  const int ret = sqlite3_bind_blob( getPreparedStatement(), aIndex, aValue, aSize, SQLITE_STATIC );
  check( ret );
}

void Statement::bindNoCopy( const char* apName, const std::string& aValue ) const {
  bindNoCopy( getIndex( apName ), aValue );
}

void Statement::bindNoCopy( const char* apName, const char* aValue ) const { bindNoCopy( getIndex( apName ), aValue ); }

void Statement::bindNoCopy( const char* apName, const char* aValue, const int aSize ) const {
  bindNoCopy( getIndex( apName ), aValue, aSize );
}

void Statement::bindNoCopy( const std::string& apName, const std::string& aValue ) const {
  bindNoCopy( getIndex( apName.c_str() ), aValue );
}

void Statement::bindNoCopy( const std::string& apName, const char* aValue ) const {
  bindNoCopy( getIndex( apName.c_str() ), aValue );
}

void Statement::bindNoCopy( const std::string& apName, const void* aValue, const int aSize ) const {
  bindNoCopy( getIndex( apName.c_str() ), aValue, aSize );
}

bool Statement::executeStep() {
  // on row or no (more) row ready, else its problem
  if ( const int ret = tryExecuteStep(); ( SQLITE_ROW != ret ) && ( SQLITE_DONE != ret ) ) {
    if ( ret == sqlite3_errcode( m_pSQLite ) ) {
      throw SQLite::Exception( m_pSQLite, ret );
    } else {
      throw SQLite::Exception( "Statement needs to be reseted", ret );
    }
  }
  return m_HasRow;
}

int Statement::tryExecuteStep() noexcept {
  if ( m_Done ) {
    return SQLITE_MISUSE;  // Statement needs to be reseted!
  }

  const int ret = sqlite3_step( m_pPreparedStatement.get() );

  if ( SQLITE_ROW == ret )  // one row is ready: call getColumn(N) to access it
  {
    m_HasRow = true;
  } else {
    m_HasRow = false;
    m_Done = SQLITE_DONE == ret;  // check if the query has finished executing
  }
  return ret;
}

int Statement::exec() {
  if ( const int ret = tryExecuteStep(); SQLITE_DONE != ret )  // the statement has finished executing successfully
  {
    if ( ret == SQLITE_ROW ) {
      throw SQLite::Exception( "exec() does not expect results.Use execute Step" );
    } else if ( ret == sqlite3_errcode( m_pSQLite ) ) {
      throw SQLite::Exception( m_pSQLite, ret );
    } else {
      throw SQLite::Exception( "Statement needs to be reseted", ret );
    }
  }

  // Return the number of rows modified by those SQL statements (INSERT, UPDATE or DELETE)
  return sqlite3_changes( m_pSQLite );
}

// test if the column is NULL
bool Statement::isColumnNull( const int aIndex ) const {
  checkRow();
  checkIndex( aIndex );
  return ( SQLITE_NULL == sqlite3_column_type( getPreparedStatement(), aIndex ) );
}

bool Statement::isColumnNull( const char* apName ) const {
  checkRow();
  const int index = getColumnIndex( apName );
  return ( SQLITE_NULL == sqlite3_column_type( getPreparedStatement(), index ) );
}

const char* Statement::getColumnName( const int aIndex ) const {
  checkIndex( aIndex );
  return sqlite3_column_name( getPreparedStatement(), aIndex );
}

#ifdef SQLITE_ENABLE_COLUMN_METADATA
const char* Statement::getColumnOriginName( const int aIndex ) const {
  checkIndex( aIndex );
  return sqlite3_column_origin_name( getPreparedStatement(), aIndex );
}
#endif

int Statement::getColumnIndex( const char* apName ) const {
  // Build the map of column index by name on first call
  if ( m_ColumNames.empty() ) {
    for ( int i = 0; i < m_ColumnCount; ++i ) {
      const char* pName = sqlite3_column_name( getPreparedStatement(), i );
      m_ColumNames[pName] = i;
    }
  }

  const auto iIndex = m_ColumNames.find( apName );
  if ( iIndex == m_ColumNames.end() ) {
    throw SQLite::Exception( "Unknown column name." );
  }

  return iIndex->second;
}

const char* Statement::getColumnDeclaredType( const int aIndex ) const {
  checkIndex( aIndex );
  const char* result = sqlite3_column_decltype( getPreparedStatement(), aIndex );
  if ( !result ) {
    throw SQLite::Exception( "Couldnt determine declared columnt type." );
  } else {
    return result;
  }
}

int Statement::getChanges() const noexcept { return sqlite3_changes( m_pSQLite ); }

const std::string& Statement::getQuery() const { return m_Query; }

// Return a UTF-8 string containing the SQL text of prepared statement with bound parameters expanded.
std::string Statement::getExpandedSQL() const {
  char* expanded = sqlite3_expanded_sql( getPreparedStatement() );
  std::string expandedString( expanded );
  sqlite3_free( expanded );
  return expandedString;
}

/// Return the number of columns in the result set returned by the prepared statement
int Statement::getColumnCount() const { return m_ColumnCount; }

/// true when a row has been fetched with executeStep()
bool Statement::hasRow() const { return m_HasRow; }

/// true when the last executeStep() had no more row to fetch
bool Statement::isDone() const { return m_Done; }

/// Return the number of bind parameters in the statement
int Statement::getBindParameterCount() const noexcept {
  return sqlite3_bind_parameter_count( m_pPreparedStatement.get() );
}

/// Return the numeric result code for the most recent failed API call (if any).
int Statement::getErrorCode() const noexcept { return sqlite3_errcode( m_pSQLite ); }

/// Return the extended numeric result code for the most recent failed API call (if any).
int Statement::getExtendedErrorCode() const noexcept { return sqlite3_extended_errcode( m_pSQLite ); }

/// Return UTF-8 encoded English language explanation of the most recent failed API call (if any).
const char* Statement::getErrorMsg() const noexcept { return sqlite3_errmsg( m_pSQLite ); }

// private functions
void Statement::check( const int aRet ) const {
  if ( OK != aRet ) {
    throw SQLite::Exception( m_pSQLite, aRet );
  }
}

void Statement::checkRow() const {
  if ( false == m_HasRow ) {
    throw SQLite::Exception( "No row to get a column from. executeStep() was not called, or returned false" );
  }
}

void Statement::checkIndex( const int aIndex ) const {
  if ( ( aIndex < 0 ) || ( aIndex >= m_ColumnCount ) ) {
    throw SQLite::Exception( "Column index out of range." );
  }
}

Statement::TStatementPtr Statement::prepareStatement() {
  sqlite3_stmt* statement;

  if ( const int ret =
           sqlite3_prepare_v2( m_pSQLite, m_Query.c_str(), static_cast<int>( m_Query.size() ), &statement, nullptr );
       OK != ret ) {
    throw SQLite::Exception( m_pSQLite, ret );
  }
  return Statement::TStatementPtr( statement, []( sqlite3_stmt* stmt ) { sqlite3_finalize( stmt ); } );
}

sqlite3_stmt* Statement::getPreparedStatement() const {
  if ( sqlite3_stmt* ret = m_pPreparedStatement.get(); ret ) {
    return ret;
  }
  throw SQLite::Exception( "Statement was not prepared." );
}

Column Statement::getColumn( const int aIndex ) const {
  checkRow();
  checkIndex( aIndex );

  // Share the Statement Object handle with the new Column created
  return Column { m_pPreparedStatement, aIndex };
}

// Return a copy of the column data specified by its column name starting at 0
// (use the Column copy-ctor)
Column Statement::getColumn( const char* apName ) const {
  checkRow();
  const int index = getColumnIndex( apName );

  // Share the statement object handle with the new Column created
  return Column( m_pPreparedStatement, index );
}

}  // namespace SQLite
