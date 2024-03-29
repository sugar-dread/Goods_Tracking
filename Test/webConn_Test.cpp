#include "webconn.h"
#include <gtest/gtest.h>

using namespace GoodsTrack;

class WebConnTest : public testing::Test {
 public:
  void SetUp() override;
  void TearDown() override;
  static void SetUpTestCase();     // called only once at the begining test
  static void TearDownTestCase();  // called only once at the end test
};

void WebConnTest::SetUp() {}
void WebConnTest::TearDown() {}
void WebConnTest::SetUpTestCase() {}
void WebConnTest::TearDownTestCase() {}

TEST_F( WebConnTest, ReadStockPrice ) {
  EXPECT_NO_THROW( WebConn {}( "PLTR" ) );
  EXPECT_THROW( WebConn {}( "PLTRRRR" ), SQLite::Exception );
  EXPECT_NO_THROW( WebConn {}( "AAPL" ) );
  EXPECT_THROW( WebConn {}( "AAPLLLL" ), SQLite::Exception );
  EXPECT_NO_THROW( WebConn {}( "TSLA" ) );
  EXPECT_THROW( WebConn {}( "TTTTTSLA" ), SQLite::Exception );

  EXPECT_EQ( "USD", WebConn {}( "PLTR" ).second );
  EXPECT_EQ( "TRY", WebConn {}( "RYGYO.IS" ).second );
}

TEST_F( WebConnTest, ReadStockPriceWithObj ) {
  WebConn obj {};
  EXPECT_NO_THROW( obj( "PLTR" ) );
  EXPECT_THROW( obj( "PLTRRRR" ), SQLite::Exception );
  EXPECT_NO_THROW( obj( "AAPL" ) );
  EXPECT_THROW( obj( "AAPLLLL" ), SQLite::Exception );
  EXPECT_NO_THROW( obj( "TSLA" ) );
  EXPECT_THROW( obj( "TTTTTSLA" ), SQLite::Exception );

  EXPECT_EQ( "USD", obj( "PLTR" ).second );
  EXPECT_EQ( "TRY", obj( "RYGYO.IS" ).second );
}
