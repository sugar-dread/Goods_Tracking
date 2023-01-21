#pragma once
#include <cassert>

/**
 * @brief SQLITECPP is used in destructors, where exceptions shall not be thrown.
 * Define SQLITECPP_ENABLE_ASSERT_HANDLER at the project level and define
 * a SQLite::assertion_failed() as sertion handler to tell SQLLiteCpp to use it instead of assert()
 * when an assertion fail.
 */

#ifdef SQLITECPP_ENABLE_ASSERT_HANDLER

// if an assert handler is provided by user code, use it instead of assert()
namespace SQLite {
// declariton of the assert handler to define in user code
void assertion_failed( const char* apFile, const int apLine, const char* apFunc, const char* apExpr,
                       const char* apMsg );

// call the assert handler provided by user code
#define SQLITECPP_ASSERT( expression, message ) \
  if ( !( expression ) ) SQLite::assertion_failed( __FILE__, __LINE__, __func__, #expression, message )

}  // namespace SQLite

#else

// if no assert handler provided by user code, use standart assert()
// (note: in release mode assert() does nothing)
#define SQLITECPP_ASSERT( expression, message ) assert( expression&& message )

#endif