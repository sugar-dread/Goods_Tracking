/**
 * @file Savepoint.h
 * @author your name (you@domain.com)
 * @brief A savepoint is a way to group multiple SQL statements into an atomic
 * secured operation. Similar to a transaction while allowing child savepoints
 * @version 0.1
 * @date 2022-11-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "Exception.h"

namespace SQLite {

// Forwarda declaration
class Database;

/**
 * @brief RAII encapsulation of a SQLite Savepoint.
 *
 * A Savepoint is a way to group multiple SQL statements into an atomic
 * secure operation; either it succeeds, with all the changes committed to the
 * database file, or if it fails, all the changes are rolled back to the initial
 * state at the start of the savepoint.
 *
 * This method also offers big performances improvements compared to
 * individually executed statements.
 *
 * Caveats:
 *
 * 1) Calling COMMIT or commiting a parent transaction or RELEASE on a parent
 * savepoint will cause this savepoint to be released.
 *
 * 2) Calling ROLLBACK or rolling back a parent savepoint will cause this
 * savepoint to be rolled back.
 *
 * 3) This savepoint is not saved to the database until this and all savepoints
 * or transaction in the savepoint stack have been released or committed.
 *
 * See also: https://sqlite.org/lang_savepoint.html
 *
 * Thread-safety: a Transaction object shall not be shared by multiple threads,
 * because:
 *
 * 1) in the SQLite "Thread Safe" mode, "SQLite can be safely used by multiple
 * threads provided that no single database connection is used simultaneously in
 * two or more threads."
 *
 * 2) the SQLite "Serialized" mode is not supported by SQLiteC++, because of the
 * way it shares the underling SQLite precompiled statement in a custom shared
 * pointer (See the inner class "Statement::Ptr").
 */
class Savepoint {
  public:
    /**
     * @brief Begins the SQLite savepoint
     *
     * @param[in] aDatabase the SQLite Database Connection
     * @param[in] aName the name of the Savepoint
     *
     * Exception is thrown in case of error, then the Savepoint is NOT
     * initiated.
     */
    Savepoint( Database& aDatabase, const std::string& aName );

    // Savepoint is non-copyable
    Savepoint( const Savepoint& ) = delete;
    Savepoint& operator=( const Savepoint& ) = delete;

    /**
     * @brief Safely rollback the savepoint if it has not been commited.
     */
    ~Savepoint();

    /**
     * @brief Commit and release the savepoint
     *
     */
    void release();

    /**
     * @brief Rollback the savepoint
     *
     */
    void rollback();

  private:
    Database& m_Database;       // reference to the SQLite Database Connection
    std::string m_Name;         // Name of savepoint
    bool m_Released { false };  // True when release has been called
};

}  // namespace SQLite
