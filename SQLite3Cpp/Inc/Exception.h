#pragma once

#include <stdexcept>
#include <string>

// Forward declaration to avoid inclusion of <sqlite3.h> in a header
struct sqlite3;

namespace SQLite {
/**
 * @brief this class is the encapsulation of the standard SQLite3 library,
 * based on std::runtime_error
 *
 */


class Exception : public std::runtime_error {
  public:
    /**
     * @brief Construct a new Exception object
     *
     * @param aErrorMessage The string message describing the SQLite Error
     * @param ret           Return value from the function call that failed.
     */
    Exception( const char* aErrorMessage, int ret );
    /**
     * @brief Construct a new Exception object with string
     *
     * @param aErrorMessage string type error message for construction
     * @param ret           Return value from the function call that failed.
     */
    Exception( const std::string& aErrorMessage, int ret );
    /**
     * @brief Construct a new Exception object
     *
     * @param aErrorMessage char type error message from SQLite3
     */
    explicit Exception( const char* aErrorMessage );
    /**
     * @brief Construct a new Exception object
     *
     * @param aErrorMessage string type error message from SQLite3
     */
    explicit Exception( const std::string& aErrorMessage );

    /**
     * @brief Construct a new Exception object
     *
     * @param apSQLite The SQLite object, to obtain detailed error message from.
     */

    explicit Exception( sqlite3* apSQLite );

    /**
     * @brief Construct a new Exception object
     *
     * @param apSQLite The SQLite object, to obtain detailed error message from.
     * @param ret      Return value from the function call that failed.
     */
    Exception( sqlite3* apSQLite, int ret );

    /**
     * @brief Get the Error Code object
     *
     * @return int returns m_ErrorCode
     */
    int getErrorCode() const noexcept;

    /**
     * @brief Get the Extended Error Code object
     *
     * @return int m_ExtendedErrCode value
     */
    int getExtendedErrorCode() const noexcept;

    /**
     * @brief Get the Error Str object
     *
     * @return const char* return the detailed description of the error.
     */
    const char* getErrorStr() const noexcept;

  private:
    int m_Errcode;
    int m_ExtendedErrCode { -1 };
};

}  // namespace SQLite