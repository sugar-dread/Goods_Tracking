/**
 * @file Backup.cpp
 * @author your name (you@domain.com)
 * @brief source file of Backup.h
 * @version 0.1
 * @date 2022-11-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Backup.h"
#include "Exception.h"

namespace SQLite {

Backup::Backup( const Database& aDestDatabase, const char* apDestDatabaseName, const Database& aSrcDatabase,
                const char* apSrcDatabaseName ) {
  m_pSQLiteBackup.reset( sqlite3_backup_init( aDestDatabase.getHandle(), apDestDatabaseName, aSrcDatabase.getHandle(),
                                              apSrcDatabaseName ) );

  if ( nullptr == m_pSQLiteBackup ) {
    // if an error occırs, the error code and message are attached to the destination database connection
    throw SQLite::Exception( aDestDatabase.getHandle() );
  }
}

Backup::Backup( const Database& aDestDatabase, const std::string& apDestDatabaseName, const Database& aSrcDatabase,
                const std::string& apSrcDatabaseName )
    : Backup( aDestDatabase, apDestDatabaseName.c_str(), aSrcDatabase, apSrcDatabaseName.c_str() ) {}

Backup::Backup( const Database& aDestDatabase, const Database& aSrcDatabase )
    : Backup( aDestDatabase, "main", aSrcDatabase, "main" ) {}

int Backup::executeStep( const int aNumPage ) const {
  const int res = sqlite3_backup_step( m_pSQLiteBackup.get(), aNumPage );
  if ( SQLITE_OK != res && SQLITE_DONE != res && SQLITE_BUSY != res && SQLITE_LOCKED != res ) {
    throw SQLite::Exception( sqlite3_errstr( res ), res );
  }
  return res;
}

int Backup::getRemainingPageCount() const { return sqlite3_backup_remaining( m_pSQLiteBackup.get() ); }

int Backup::getTotalPageCount() const { return sqlite3_backup_pagecount( m_pSQLiteBackup.get() ); }

void SQLite::Backup::Deleter::operator()( sqlite3_backup* apBackup ) const {
  if ( apBackup ) {
    sqlite3_backup_finish( apBackup );
  }
}

}  // namespace SQLite