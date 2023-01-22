#include <gtest/gtest.h>
#include <iostream>
#include "goods_track.h"
#include "menu_utility.h"

using namespace GoodsTrack;

class GoodsTrackTest : public testing::Test {
 public:
  void SetUp() override;
  void TearDown() override;
  static void SetUpTestCase();     // called only once at the begining test
  static void TearDownTestCase();  // called only once at the end test
};

void GoodsTrackTest::SetUp() {}
void GoodsTrackTest::TearDown() {}
void GoodsTrackTest::SetUpTestCase() {}
void GoodsTrackTest::TearDownTestCase() {}

TEST_F( GoodsTrackTest, OpenCreateDbTest ) {
  std::remove( ( getProjectCurrentPath() + "/goods.db" ).c_str() );

  ASSERT_FALSE( std::filesystem::exists( getProjectCurrentPath() + "/goods.db" ) );

  auto db = open_goods_database( "goods.db" );
  ASSERT_TRUE( std::filesystem::exists( getProjectCurrentPath() + "/goods.db" ) );
  ASSERT_TRUE( db.tableExists( "goods" ) );

  std::remove( ( getProjectCurrentPath() + "/goods.db" ).c_str() );
}

TEST_F( GoodsTrackTest, AddGoodsDbTest ) {
  auto db = open_goods_database( "goods.db" );

  GoodsManager obj( db, "goods" );
  obj.add_goods( GoodsElems { 1, "gold", 3, 2200.2, 36000, "TRY" } );
  obj.add_goods( GoodsElems { 2, "silver", 4, 5200.2, 46000, "TRY" } );
  obj.add_goods( GoodsElems { 3, "bronz", 5, 2200.2, 56000, "TRY" } );

  auto Goods = obj.get_goods( 1 );
  EXPECT_EQ( Goods.getId(), 1 );
  EXPECT_EQ( Goods.getName(), "gold" );
  EXPECT_EQ( Goods.getAmount(), 3 );
  EXPECT_EQ( Goods.getPrice(), 2200.2 );
  EXPECT_EQ( Goods.getTotal(), 36000 );
  EXPECT_EQ( Goods.getCurrency(), "TRY" );

  Goods = obj.get_goods( 2 );
  EXPECT_EQ( Goods.getId(), 2 );
  EXPECT_EQ( Goods.getName(), "silver" );
  EXPECT_EQ( Goods.getAmount(), 4 );
  EXPECT_EQ( Goods.getPrice(), 5200.2 );
  EXPECT_EQ( Goods.getTotal(), 46000 );
  EXPECT_EQ( Goods.getCurrency(), "TRY" );

  Goods = obj.get_goods( 3 );
  EXPECT_EQ( Goods.getId(), 3 );
  EXPECT_EQ( Goods.getName(), "bronz" );
  EXPECT_EQ( Goods.getAmount(), 5 );
  EXPECT_EQ( Goods.getPrice(), 2200.2 );
  EXPECT_EQ( Goods.getTotal(), 56000 );
  EXPECT_EQ( Goods.getCurrency(), "TRY" );

  EXPECT_THROW( obj.get_goods( 4 ), SQLite::Exception );
}

TEST_F( GoodsTrackTest, UpdateAmountGoodsById ) {
  auto db = open_goods_database( "goods.db" );
  GoodsManager obj( db, "goods" );

  auto Goods = obj.get_goods( 1 );
  obj.update_with_id( 1, 40 );
  Goods = obj.get_goods( 1 );
  EXPECT_EQ( Goods.getAmount(), 40 );

  Goods = obj.get_goods( 2 );
  obj.update_with_id( 2, 50 );
  Goods = obj.get_goods( 2 );
  EXPECT_EQ( Goods.getAmount(), 50 );

  Goods = obj.get_goods( 3 );
  obj.update_with_id( 3, 60 );
  Goods = obj.get_goods( 3 );
  EXPECT_EQ( Goods.getAmount(), 60 );
}

TEST_F( GoodsTrackTest, DeleteGoodsById ) {
  auto db = open_goods_database( "goods.db" );
  GoodsManager obj( db, "goods" );

  obj.delete_with_id( 2 );
  EXPECT_THROW( obj.get_goods( 2 ), SQLite::Exception );

  obj.delete_with_id( 1 );
  EXPECT_THROW( obj.get_goods( 1 ), SQLite::Exception );
}

TEST_F( GoodsTrackTest, ReadGoodsYahooFinance ) {
  auto db = open_goods_database( "goods.db" );
  GoodsManager obj( db, "goods" );

  obj.add_goods( obj.create_goods( "PLTR", 500 ) );
  auto Goods = obj.get_goods( 4 );
  EXPECT_EQ( Goods.getId(), 4 );
  EXPECT_EQ( Goods.getName(), "PLTR" );
  EXPECT_EQ( Goods.getAmount(), 500 );
  EXPECT_EQ( Goods.getCurrency(), "USD" );

  obj.add_goods( obj.create_goods( "RYGYO.IS", 501 ) );
  Goods = obj.get_goods( 5 );
  EXPECT_EQ( Goods.getId(), 5 );
  EXPECT_EQ( Goods.getName(), "RYGYO.IS" );
  EXPECT_EQ( Goods.getAmount(), 501 );
  EXPECT_EQ( Goods.getCurrency(), "TRY" );

  obj.add_goods( obj.create_goods( "GC=F", 1 ) );
  Goods = obj.get_goods( 6 );
  EXPECT_EQ( Goods.getId(), 6 );
  EXPECT_EQ( Goods.getName(), "GC=F" );
  EXPECT_EQ( Goods.getAmount(), 1 );
  EXPECT_EQ( Goods.getCurrency(), "USD" );

  obj.add_goods( obj.create_goods( "SI=F", 1 ) );
  Goods = obj.get_goods( 7 );
  EXPECT_EQ( Goods.getId(), 7 );
  EXPECT_EQ( Goods.getName(), "SI=F" );
  EXPECT_EQ( Goods.getAmount(), 1 );
  EXPECT_EQ( Goods.getCurrency(), "USD" );

  EXPECT_THROW( obj.add_goods( obj.create_goods( "RYGYO", 500 ) ), SQLite::Exception );
}

TEST_F( GoodsTrackTest, GetTotalWealth ) {
  auto db = open_goods_database( "goods.db" );
  GoodsManager obj( db, "goods" );

  obj.calculate_total_wealth( "USD" );

  print_good_db( db, obj, "TRY" );
}