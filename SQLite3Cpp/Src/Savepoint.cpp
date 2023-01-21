/**
 * @file Savepoint.cpp
 * @author your name (you@domain.com)
 * @brief A source file of savepoint.h
 * @version 0.1
 * @date 2022-11-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Savepoint.h"
#include "Assertion.h"
#include "Database.h"
#include "Statement.h"

namespace SQLite {
Savepoint::Savepoint( Database& aDatabase, const std::string& aName ) : m_Database { aDatabase }, m_Name( aName ) {
  // workaround because you cannot bind to SAVEPOINT escape name for use in query
  Statement stmt( m_Database, "SELECT quote(?)" );
  stmt.bind( 1, m_Name );
  stmt.executeStep();
  m_Name = stmt.getColumn( 0 ).getText();
  m_Database.exec( std::string( "SAVEPOINT " ) + m_Name );
}

// Safely rollback the savepoint if it has not been committed.
Savepoint::~Savepoint() {
  if ( !m_Released ) {
    try {
      rollback();
    } catch ( SQLite::Exception& ) {
      // Never throw an exception in a destructor: error if already rolled back or released, but no harm is caused by
      // this.
    }
  }
}

// Releae the savepoint and commit
void Savepoint::release() {
  if ( !m_Released ) {
    m_Database.exec( std::string( "RELEASE SAVEPOINT " ) + m_Name );
    m_Released = true;
  } else {
    throw SQLite::Exception( "Savepoint already released or rolled back." );
  }
}

// Rollback the savepoint
void Savepoint::rollback() {
  if ( !m_Released ) {
    m_Database.exec( std::string( "ROLLBACK TO SAVEPOINT " ) + m_Name );
    m_Released = true;
  } else {
    throw SQLite::Exception( "Savepoint already released or rolled back." );
  }
}

}  // namespace SQLite