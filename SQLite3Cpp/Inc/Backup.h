/**
 * @file Backup.h
 * @author your name (you@domain.com)
 * @brief backup is used to backup a database file in a safe and online way
 * @version 0.1
 * @date 2022-11-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "Database.h"

#include <sqlite3.h>
#include <memory>
#include <string>

namespace SQLite {

/**
 * @brief RAII encapsulation of a SQLite Database Backup process.
 *
 * A Backup object is used to backup a source database file to a destination database file
 * in a safe and online way.
 *
 * See also the a reference implementation of live backup taken from the official site:
 * https://www.sqlite.org/backup.html
 */
class Backup {
  public:
    /**
     * @brief Initialize a SQLite Backup object.
     *
     * Initialize a SQLite Backup object for the source database and destination database.
     * The database name is "main" for the main database, "temp" for the temporary database,
     * or the name specified after the AS keyword in an ATTACH statement for an attached database.
     *
     * Exception is thrown in case of error, then the Backup object is NOT constructed.
     *
     * @param[in] aDestDatabase        Destination database connection
     * @param[in] apDestDatabaseName   Destination database name
     * @param[in] aSrcDatabase         Source database connection
     * @param[in] apSrcDatabaseName    Source database name
     *
     * @throw SQLite::Exception in case of error
     */
    Backup( const Database& aDestDatabase, const char* apDestDatabaseName, const Database& aSrcDatabase,
            const char* apSrcDatabaseName );
    Backup( const Database& aDestDatabase, const std::string& apDestDatabaseName, const Database& aSrcDatabase,
            const std::string& apSrcDatabaseName );
    Backup( const Database& aDestDatabase, const Database& aSrcDatabase );

    // Backup is non-copyable
    Backup( const Backup& ) = delete;
    Backup& operator=( const Backup& ) = delete;

    /**
     * @brief Execute a step of backup with a given number of source pages to be copied
     *
     * Exception is thrown when SQLITE_IOERR_XXX, SQLITE_NOMEM, or SQLITE_READONLY is returned
     * in sqlite3_backup_step(). These errors are considered fatal, so there is no point
     * in retrying the call to executeStep().
     *
     * @param[in] aNumPage    The number of source pages to be copied, with a negative value meaning all remaining
     * source pages
     *
     * @return SQLITE_OK/SQLITE_DONE/SQLITE_BUSY/SQLITE_LOCKED
     *
     * @throw SQLite::Exception in case of error
     */
    int executeStep( const int aNumPage = -1 ) const;

    /// Return the number of source pages still to be backed up as of the most recent call to executeStep().
    int getRemainingPageCount() const;

    /// Return the total number of pages in the source database as of the most recent call to executeStep().
    int getTotalPageCount() const;

  private:
    struct Deleter {
        void operator()( sqlite3_backup* apBackup ) const;
    };
    std::unique_ptr<sqlite3_backup, Deleter> m_pSQLiteBackup {};
};
}  // namespace SQLite