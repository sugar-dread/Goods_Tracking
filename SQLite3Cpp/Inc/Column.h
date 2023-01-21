/**
 * @file Column.h
 * @author your name (you@domain.com)
 * @brief Encapsulation of a Column in a row the result point by the prepared SQLite::Statement
 * @version 0.1
 * @date 2022-11-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "Statement.h"
#include "Exception.h"

#include <string>
#include <memory>

namespace SQLite {
constexpr int INTEGER { SQLITE_INTEGER };
constexpr int FLOAT { SQLITE_FLOAT };
constexpr int TEXT { SQLITE_TEXT };
constexpr int BLOB { SQLITE_BLOB };
constexpr int Null { SQLITE_NULL };

/**
 * @brief Encapsulation of a Column in a row of the result pointed by the prepared Statement.
 *
 *  A Column is a particular field of SQLite data in the current row of result
 * of the Statement : it points to a single cell.
 *
 * Its value can be expressed as a text, and, when applicable, as a numeric
 * (integer or floating point) or a binary blob.
 *
 * Thread-safety: a Column object shall not be shared by multiple threads, because :
 * 1) in the SQLite "Thread Safe" mode, "SQLite can be safely used by multiple threads
 *    provided that no single database connection is used simultaneously in two or more threads."
 * 2) the SQLite "Serialized" mode is not supported by SQLiteC++,
 *    because of the way it shares the underling SQLite precompiled statement
 *    in a custom shared pointer (See the inner class "Statement::Ptr").
 */

class Column {
  public:
    /**
     * @brief Encapsulation of a Column in a Row of the result.
     *
     * @param[in] aStmPtr  Shared pointer to the prepared SQLite Statement Object.
     * @param[in] aIndex    Index of the column in the row of result, starting at 0
     */
    explicit Column( const Statement::TStatementPtr& aStmPtr, int aIndex );

    /**
     * @brief Return a pointer to the named assigned to this result column (potentially aliased)
     *
     * @see getOriginName() to get original column name (not aliased)
     */
    const char* getName() const noexcept;

#ifdef SQLITE_ENABLE_COLUMN_METADATA
    /**
     * @brief Return a pointer to the table column name that is the origin of this result column
     *
     *  Require definition of the SQLITE_ENABLE_COLUMN_METADATA preprocessor macro :
     * - when building the SQLite library itself (which is the case for the Debian libsqlite3 binary for instance),
     * - and also when compiling this wrapper.
     */
    const char* getOriginName() const noexcept;
#endif

    /// Return the integer value of the column.
    int32_t getInt() const noexcept;

    /// Return the 32bits unsigned integer value of the column (note that SQLite3 does not support unsigned 64bits).
    uint32_t getUInt() const noexcept;

    /// Return the 64bits integer value of the column (note that SQLite3 does not support unsigned 64bits).
    int64_t getInt64() const noexcept;

    /// Return the double (64bits float) value of the column
    double getDouble() const noexcept;

    /**
     * @brief Return a pointer to the text value (NULL terminated string) of the column.
     *
     * @warning The value pointed at is only valid while the statement is valid (ie. not finalized),
     *          thus you must copy it before using it beyond its scope (to a std::string for instance).
     */
    const char* getText( const char* apDefaultValue = "" ) const noexcept;

    /**
     * @brief Return a pointer to the binary blob value of the column.
     *
     * @warning The value pointed at is only valid while the statement is valid (ie. not finalized),
     *          thus you must copy it before using it beyond its scope (to a std::string for instance).
     */
    const void* getBlob() const noexcept;

    /**
     * @brief Return a std::string for a TEXT or BLOB column.
     *
     * Note this correctly handles strings that contain null bytes.
     */
    std::string getString() const;

    /**
     * @brief Return the type of the value of the column
     *
     * Return either SQLite::INTEGER, SQLite::FLOAT, SQLite::TEXT, SQLite::BLOB, or SQLite::Null.
     *
     * @warning After a type conversion (by a call to a getXxx on a Column of a Yyy type),
     *          the value returned by sqlite3_column_type() is undefined.
     */
    int getType() const noexcept;

    /**
     * @brief Return the number of bytes used by the text (or blob) value of the column
     *
     * Return either :
     * - size in bytes (not in characters) of the string returned by getText() without the '\0' terminator
     * - size in bytes of the string representation of the numerical value (integer or double)
     * - size in bytes of the binary blob returned by getBlob()
     * - 0 for a NULL value
     */
    int getBytes() const noexcept;

    bool isInteger() const noexcept { return SQLite::INTEGER == getType(); }

    bool isFloat() const noexcept { return SQLite::FLOAT == getType(); }

    bool isText() const noexcept { return SQLite::TEXT == getType(); }

    bool isBlob() const noexcept { return SQLite::BLOB == getType(); }

    bool isNull() const noexcept { return SQLite::Null == getType(); }

        /// Alias returning the number of bytes used by the text (or blob) value of the column
    int size() const noexcept { return getBytes(); }

    // Inline cast operators to basic types
    operator char() const { return static_cast<char>( getInt() ); }

    operator int8_t() const { return static_cast<int8_t>( getInt() ); }

    operator uint8_t() const { return static_cast<uint8_t>( getInt() ); }

    operator int16_t() const { return static_cast<int16_t>( getInt() ); }

    operator uint16_t() const { return static_cast<uint16_t>( getInt() ); }

    operator int32_t() const { return getInt(); }

    operator uint32_t() const { return getUInt(); }

    operator int64_t() const { return getInt64(); }

    operator double() const { return getDouble(); }

    operator const char*() const { return getText(); }

    operator const void*() const { return getBlob(); }

    operator std::string() const { return getString(); }

  private:
    Statement::TStatementPtr m_StmPtr;  //<Shared pointer to the prepared SQLite Statement Object
    int m_Index;                        //< Index of the column in the row result, starting at 0
};
std::ostream& operator<<( std::ostream& aStream, const Column& aColumn );

template <typename T, int N>
T Statement::getColumns() {
  checkRow();
  checkIndex( N - 1 );
  return getColumns<T>( std::make_integer_sequence<int, N> {} );
}

template <typename T, const int... Is>
T Statement::getColumns( const std::integer_sequence<int, Is...> ) {
  return T { Column( m_pPreparedStatement, Is )... };
}

}  // namespace SQLite