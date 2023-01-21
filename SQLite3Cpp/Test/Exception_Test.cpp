/**
 * @file Exception_Test.cpp
 * @author your name (you@domain.com)
 * @brief Unit test file of a SQlite Exception
 * @version 0.1
 * @date 2022-11-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "Exception.h"
#include <gtest/gtest.h>
#include <string>

TEST( Exception, CopyTest ) {
  const SQLite::Exception ex1( "some error", 2 );
  const SQLite::Exception ex2 = ex1;
  EXPECT_STREQ( ex1.what(), ex2.what() );
  EXPECT_EQ( ex1.getErrorCode(), ex2.getErrorCode() );
  EXPECT_EQ( ex1.getExtendedErrorCode(), ex2.getExtendedErrorCode() );
}

TEST( Exception, AssignmentTest ) {
  const std::string message { "some error" };
  const SQLite::Exception ex1( message, 1 );
  SQLite::Exception ex2( "another error", 2 );
  ex2 = ex1;

  EXPECT_STREQ( ex2.what(), message.c_str() );
  EXPECT_EQ( ex2.getErrorCode(), 1 );
}

TEST( Exception, ThrowCatchTest ) {
  const char message[] = "some error";
  try {
    throw SQLite::Exception( message );
  } catch ( const std::runtime_error& ex ) {
    EXPECT_STREQ( ex.what(), message );
  }
}

TEST( Exception, ConstructorTest ) {
  const char msg1[] = "some error";
  std::string msg2 = "another error";
  {
    const SQLite::Exception ex { msg1 };
    EXPECT_STREQ( ex.what(), msg1 );
    EXPECT_EQ( ex.getErrorCode(), -1 );
    EXPECT_EQ( ex.getExtendedErrorCode(), -1 );
    EXPECT_STREQ( "unknown error", ex.getErrorStr() );
  }
  {
    const SQLite::Exception ex { msg2 };
    EXPECT_STREQ( ex.what(), msg2.c_str() );
    EXPECT_EQ( ex.getErrorCode(), -1 );
    EXPECT_EQ( ex.getExtendedErrorCode(), -1 );
    EXPECT_STREQ( "unknown error", ex.getErrorStr() );
  }

  {
    const SQLite::Exception ex { msg1, 1 };
    EXPECT_STREQ( ex.what(), msg1 );
    EXPECT_EQ( ex.getErrorCode(), 1 );
    EXPECT_EQ( ex.getExtendedErrorCode(), -1 );
    EXPECT_STREQ( "SQL logic error", ex.getErrorStr() );
  }

  {
    const SQLite::Exception ex { msg2, 2 };
    EXPECT_STREQ( ex.what(), msg2.c_str() );
    EXPECT_EQ( ex.getErrorCode(), 2 );
    EXPECT_EQ( ex.getExtendedErrorCode(), -1 );
    EXPECT_STREQ( "unknown error", ex.getErrorStr() );
  }
}