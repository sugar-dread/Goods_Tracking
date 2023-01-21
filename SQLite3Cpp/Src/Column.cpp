/**
 * @file Column.cpp
 * @author your name (you@domain.com)
 * @brief the source code of Column.h
 * @version 0.1
 * @date 2022-11-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Column.h"
#include <sqlite3.h>
#include <iostream>

namespace SQLite {
Column::Column( const Statement::TStatementPtr& aStmPtr, int aIndex ) : m_StmPtr( aStmPtr ), m_Index( aIndex ) {
  if ( !aStmPtr ) {
    throw SQLite::Exception( "Statement was destroyed" );
  }
}

const char* Column::getName() const noexcept { return sqlite3_column_name( m_StmPtr.get(), m_Index ); }

#ifdef SQLITE_ENABLE_COLUMN_METADATA
const char* Column::getOriginName() const noexcept { return sqlite3_column_origin_name( m_StmPtr.get(), m_Index ); }
#endif

int32_t Column::getInt() const noexcept { return sqlite3_column_int( m_StmPtr.get(), m_Index ); }

uint32_t Column::getUInt() const noexcept { return static_cast<unsigned>( getInt64() ); }

int64_t Column::getInt64() const noexcept { return sqlite3_column_int64( m_StmPtr.get(), m_Index ); }

double Column::getDouble() const noexcept { return sqlite3_column_double( m_StmPtr.get(), m_Index ); }

const char* Column::getText( const char* apDefaultValue ) const noexcept {
  const auto* pText = reinterpret_cast<const char*>( sqlite3_column_text( m_StmPtr.get(), m_Index ) );
  return ( pText ? pText : apDefaultValue );
}

std::string Column::getString() const {
  // Note: using sqlite3_column_blob and not sqlite3_column_text
  // - no need for sqlite3_column_text to add a \0 on the end, as we're getting the bytes length directly
  (void)sqlite3_column_bytes( m_StmPtr.get(), m_Index );
  auto data = static_cast<const char*>( sqlite3_column_blob( m_StmPtr.get(), m_Index ) );

  // SQLite docs: "The safest policy is to invoke… sqlite3_column_blob() followed by sqlite3_column_bytes()"
  // Note: std::string is ok to pass nullptr as first arg, if length is 0
  return std::string( data, sqlite3_column_bytes( m_StmPtr.get(), m_Index ) );
}

int Column::getType() const noexcept { return sqlite3_column_type( m_StmPtr.get(), m_Index ); }

int Column::getBytes() const noexcept { return sqlite3_column_bytes( m_StmPtr.get(), m_Index ); }

std::ostream& operator<<( std::ostream& aStream, const Column& aColumn ) {
  aStream.write( aColumn.getText(), aColumn.getBytes() );
  return aStream;
}

const void* Column::getBlob() const noexcept { return sqlite3_column_blob( m_StmPtr.get(), m_Index ); }

}  // namespace SQLite