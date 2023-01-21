/**
 * @file Exception.cpp
 * @author your name (you@domain.com)
 * @brief Source code for exception.h file
 * @version 0.1
 * @date 2022-11-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Exception.h"
#include <sqlite3.h>

namespace SQLite {
Exception::Exception( const char* aErrorMessage, int ret ) : std::runtime_error { aErrorMessage }, m_Errcode { ret } {}
Exception::Exception( const std::string& aErrorMessage, int ret ) : Exception( aErrorMessage.c_str(), ret ) {}
Exception::Exception( const char* aErrorMessage ) : Exception( aErrorMessage, -1 ) {}  // 0 would be SQLITE_OK which doesnt make sense
Exception::Exception( const std::string& aErrorMessage ) : Exception( aErrorMessage.c_str(), -1 ) {}
Exception::Exception( sqlite3* apSQLite ) : std::runtime_error( sqlite3_errmsg( apSQLite ) ), m_Errcode( sqlite3_errcode( apSQLite ) ), m_ExtendedErrCode( sqlite3_extended_errcode( apSQLite ) ) {}
Exception::Exception( sqlite3* apSQLite, int ret ) : std::runtime_error( sqlite3_errmsg( apSQLite ) ), m_Errcode( ret ), m_ExtendedErrCode( sqlite3_extended_errcode( apSQLite ) ) {}
int Exception::getErrorCode() const noexcept { return m_Errcode; }
int Exception::getExtendedErrorCode() const noexcept { return m_ExtendedErrCode; }
const char* Exception::getErrorStr() const noexcept { return sqlite3_errstr( m_Errcode ); }
}  // namespace SQLite
