#include "goods.h"
#include <gtest/gtest.h>
#include <iostream>

using namespace GoodsTrack;

class GoodsTest : public testing::Test {
 public:
  void SetUp() override;
  void TearDown() override;
  static void SetUpTestCase();     // called only once at the begining test
  static void TearDownTestCase();  // called only once at the end test
};

void GoodsTest::SetUp() {}
void GoodsTest::TearDown() {}
void GoodsTest::SetUpTestCase() {}
void GoodsTest::TearDownTestCase() {}

TEST_F( GoodsTest, CreateGoods ) {
  Goods obj { 1, "gold", 3, 2200.2, 36000.0, "TRY" };

  EXPECT_EQ( obj.getId(), 1 );
  EXPECT_EQ( obj.getName(), "gold" );
  EXPECT_EQ( obj.getAmount(), 3 );
  EXPECT_EQ( obj.getPrice(), 2200.2 );
  EXPECT_EQ( obj.getTotal(), 36000 );
  EXPECT_EQ( obj.getCurrency(), "TRY" );
}

TEST_F( GoodsTest, SetGetGoods ) {
  Goods obj { 1, "gold", 3, 2200.2, 36000.0, "TRY" };

  obj.setId( 2 );
  EXPECT_EQ( obj.getId(), 2 );

  obj.setName( "silver" );
  EXPECT_EQ( obj.getName(), "silver" );

  obj.setAmount( 5 );
  EXPECT_EQ( obj.getAmount(), 5 );

  obj.setPrice( 25.56 );
  EXPECT_EQ( obj.getPrice(), 25.56 );

  obj.setTotal( 105.68 );
  EXPECT_EQ( obj.getTotal(), 105.68 );

  obj.setCurrency( "JPY" );
  EXPECT_EQ( obj.getCurrency(), "JPY" );
}