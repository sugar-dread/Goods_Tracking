/**
 * @file goods_track.cpp
 * @author your name (you@domain.com)
 * @brief source code of goods_trach.h
 * @version 0.1
 * @date 2023-01-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "goods_track.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <utility>

namespace GoodsTrack {

constexpr double OUNCE_GRAM { 31.1034768 };

SQLite::Database open_goods_database( const std::string& file_name ) {
  auto exist_flag { false };
  if ( std::filesystem::exists( getProjectCurrentPath() + "/" + file_name ) ) {
    exist_flag = true;
  }

  int16_t mode {};
  if ( exist_flag ) {
    mode = SQLite::OPEN_READWRITE;
  } else {
    mode = SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE;
  }

  SQLite::Database db { getProjectCurrentPath() + "/" + file_name, mode };

  if ( !db.tableExists( "date_updated" ) ) {
    db.exec( "CREATE TABLE date_updated (id INTEGER PRIMARY KEY, updated_date TEXT, usd_amount DOUBLE)" );
  }

  if ( !exist_flag ) {
    db.exec( "DROP TABLE IF EXISTS goods" );
    db.exec(
        "CREATE TABLE goods (id INTEGER PRIMARY KEY, name TEXT, amount DOUBLE, price DOUBLE, "
        "total DOUBLE, currency TEXT)" );
    db.exec( "DROP TABLE IF EXISTS date_updated" );
    db.exec( "CREATE TABLE date_updated (id INTEGER PRIMARY KEY, updated_date TEXT, usd_amount DOUBLE)" );
  }

  return db;
}

void GoodsManager::add_goods( const Goods& elem ) const {
  if ( !( m_Database.tableExists( m_TableName ) ) ) {
    throw SQLite::Exception( "Table " + m_TableName + "does not exists in database" );
  }

  std::stringstream add_stmt;
  add_stmt << "INSERT INTO " + m_TableName + " VALUES " << std::fixed << std::setprecision( 3 ) << "(NULL, "
           << "\"" << elem.getName() << "\", '" << std::to_string( elem.getAmount() ) << "', " << elem.getPrice() << ", '" << std::to_string( elem.getTotal() ) << "', "
           << "\"" << elem.getCurrency() << "\")";

  m_Database.exec( add_stmt.str() );
}

Goods GoodsManager::get_goods( int id ) const {
  if ( !( m_Database.tableExists( m_TableName ) ) ) {
    throw SQLite::Exception( "Table " + m_TableName + "does not exists in database" );
  }

  Goods obj;

  std::stringstream get_goods_by_id;
  get_goods_by_id << "SELECT * FROM " << m_TableName << " WHERE "
                  << "id = " << id;

  SQLite::Statement queryCheck { m_Database, get_goods_by_id.str() };

  auto result { false };
  while ( queryCheck.executeStep() ) {
    if ( id == queryCheck.getColumn( 0 ).getInt() ) {
      result = true;
      obj.setId( queryCheck.getColumn( static_cast<int>( DB_COLUMNS::ID ) ).getInt() );
      obj.setName( queryCheck.getColumn( static_cast<int>( DB_COLUMNS::NAME ) ).getText() );
      obj.setAmount( queryCheck.getColumn( static_cast<int>( DB_COLUMNS::AMOUNT ) ).getDouble() );
      obj.setPrice( queryCheck.getColumn( static_cast<int>( DB_COLUMNS::PRICE ) ).getDouble() );
      obj.setTotal( queryCheck.getColumn( static_cast<int>( DB_COLUMNS::TOTAL ) ).getDouble() );
      obj.setCurrency( queryCheck.getColumn( static_cast<int>( DB_COLUMNS::CURRENCY ) ).getText() );
    }
  }

  if ( !result ) {
    throw SQLite::Exception( "Goods with id : " + std::to_string( id ) + " does not exists in database" );
  }
  queryCheck.reset();
  return obj;
}

Goods GoodsManager::create_goods( std::string_view symbol, double amount ) {
  auto [price, currency] = WebConn {}( symbol, m_Api_key );
  return Goods { 0, symbol.data(), amount, price, amount * price, currency };
}

int GoodsManager::get_row_number() const {
  std::stringstream get_number_of_rows;
  get_number_of_rows << "SELECT COUNT(*) FROM goods";
  SQLite::Statement queryCheck { m_Database, get_number_of_rows.str() };

  queryCheck.executeStep();
  auto number = queryCheck.getColumn( 0 ).getInt();
  queryCheck.reset();

  return number;
}

void GoodsManager::update_with_id( int id, double amount ) const {
  if ( !( m_Database.tableExists( m_TableName ) ) ) {
    throw SQLite::Exception( "Table " + m_TableName + "does not exists in database" );
  }

  std::stringstream get_goods_by_id;
  get_goods_by_id << "SELECT * FROM " << m_TableName << " WHERE "
                  << "id = " << id;

  SQLite::Statement queryCheck { m_Database, get_goods_by_id.str() };

  double get_price {};
  auto result { false };
  while ( queryCheck.executeStep() ) {
    if ( id == queryCheck.getColumn( 0 ).getInt() ) {
      get_price = queryCheck.getColumn( 3 ).getDouble();
      result = true;
    }
  }

  if ( !result ) {
    throw SQLite::Exception( "Wrong id : " + std::to_string( id ) + " row number in the database " );
  }

  std::stringstream update_goods_by_id;
  update_goods_by_id << "UPDATE " << m_TableName << " SET amount='" << std::to_string( amount ) << "', total='" << std::to_string( amount * get_price ) << "' WHERE id='" << id << "'";

  SQLite::Statement queryUpdate { m_Database, update_goods_by_id.str() };

  m_Database.exec( update_goods_by_id.str() );
  queryCheck.reset();
  queryUpdate.reset();
}

void GoodsManager::delete_with_id( int id ) const {
  if ( !( m_Database.tableExists( m_TableName ) ) ) {
    throw SQLite::Exception( "Table " + m_TableName + "does not exists in database" );
  }

  std::stringstream get_goods_by_id;
  get_goods_by_id << "SELECT * FROM " << m_TableName << " WHERE "
                  << "id = " << id;

  SQLite::Statement queryCheck { m_Database, get_goods_by_id.str() };

  auto result { false };
  while ( queryCheck.executeStep() ) {
    if ( id == queryCheck.getColumn( 0 ).getInt() ) {
      result = true;
    }
  }

  if ( !result ) {
    throw SQLite::Exception( "Wrong id : " + std::to_string( id ) + " row number in the database " );
  }

  std::stringstream delete_goods_by_id;
  delete_goods_by_id << "DELETE FROM " << m_TableName << " WHERE id='" << id << "'";

  m_Database.exec( delete_goods_by_id.str() );
}

std::vector<Goods> GoodsManager::get_all_goods() const {
  SQLite::Statement query { m_Database, ( "SELECT * FROM " + m_TableName ) };

  std::vector<Goods> cvec;

  while ( query.executeStep() ) {
    cvec.emplace_back( query.getColumn( static_cast<int>( DB_COLUMNS::ID ) ).getInt(), query.getColumn( static_cast<int>( DB_COLUMNS::NAME ) ).getText(),
                       query.getColumn( static_cast<int>( DB_COLUMNS::AMOUNT ) ).getDouble(), query.getColumn( static_cast<int>( DB_COLUMNS::PRICE ) ).getDouble(),
                       query.getColumn( static_cast<int>( DB_COLUMNS::TOTAL ) ).getDouble(), query.getColumn( static_cast<int>( DB_COLUMNS::CURRENCY ) ).getText() );
  }

  query.reset();
  return cvec;
}

double GoodsManager::calculate_total_wealth( std::string_view currency ) const {
  double sum {};

  std::string ticker {};
  WebConn obj {};

  for ( auto gvec = get_all_goods(); const auto& elem : gvec ) {
    ticker = ( currency.data() + elem.getCurrency() + "=X" );
    if ( elem.getCurrency() != currency.data() ) {
      auto currency_price = obj( ticker, m_Api_key ).first;
      sum += ( elem.getTotal() / currency_price );
    } else {
      sum += elem.getTotal();
    }
    ticker.clear();
  }

  return sum;
}

void GoodsManager::update_goods_prices() const {
  auto gvec = get_all_goods();

  std::stringstream update_goods_by_all;

  double price {};
  double amount {};
  WebConn obj {};

  for ( const auto& elem : gvec ) {
    price = obj( elem.getName(), m_Api_key ).first;
    amount = elem.getAmount();

    if ( ( elem.getName() == "SI=F" ) || ( elem.getName() == "GC=F" ) ) {
      price = price / OUNCE_GRAM;
    }

    update_goods_by_all << "UPDATE " << m_TableName << " SET price=" << price << ", total='" << std::to_string( amount * price ) << "' WHERE id='" << elem.getId() << "'";

    m_Database.exec( update_goods_by_all.str() );
    update_goods_by_all.str( "" );
    update_goods_by_all.clear();
  }
}

void GoodsManager::insert_last_updated_amount() const {
  if ( !( m_Database.tableExists( "date_updated" ) ) ) {
    throw SQLite::Exception( "Table date_updated does not exists in database" );
  }

  auto current_time = std::chrono::system_clock::now();                               // Get the current time
  std::time_t current_time_t = std::chrono::system_clock::to_time_t( current_time );  // Convert the time to a time_t object
  std::tm current_tm = *std::localtime( &current_time_t );                            // Convert the time_t object to a tm struct in local time
  std::string y = std::to_string( current_tm.tm_year + 1900 );
  std::string m = std::to_string( current_tm.tm_mon + 1 );
  std::string d = std::to_string( current_tm.tm_mday );
  std::string cur_date = y + "-" + m + "-" + d;

  auto total_amount = calculate_total_wealth( "USD" );

  std::stringstream add_stmt;
  add_stmt.clear();
  add_stmt << "INSERT INTO date_updated VALUES (NULL, "
           << "\"" << cur_date << "\", '" << std::to_string( total_amount ) << "')";

  std::cout << add_stmt.str() << "\n";
  m_Database.exec( add_stmt.str() );
}

std::vector<std::pair<std::string, double>> GoodsManager::get_updated_dates() const {
  SQLite::Statement query { m_Database, "SELECT * FROM date_updated" };

  std::vector<std::pair<std::string, double>> pvec;

  while ( query.executeStep() ) {
    pvec.emplace_back( query.getColumn( 1 ).getText(), query.getColumn( 2 ).getDouble() );
  }

  query.reset();
  return pvec;
}

std::string GoodsManager::get_api_key() {
  if ( !( m_Database.tableExists( "api_key" ) ) ) {
    throw SQLite::Exception( "Table api_key does not exists in database" );
  }

  SQLite::Statement query { m_Database, "SELECT * FROM api_key WHERE id = 1" };

  std::vector<std::pair<std::string, double>> pvec;

  if ( query.executeStep() ) {
    return query.getColumn( 1 ).getText();
  } else {
    throw SQLite::Exception( "No record found with id = 1 in api_key table" );
  }
}

}  // namespace GoodsTrack
