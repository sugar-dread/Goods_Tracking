/**
 * @file goods_track.h
 * @author your name (you@domain.com)
 * @brief This header file includes tracking functions for goods
 * @version 0.1
 * @date 2023-01-21
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once


#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/schema.h>
#include <array>
#include <filesystem>
#include <vector>
#include "SQLiteCpp.h"
#include "goods.h"

namespace GoodsTrack {

enum class DB_COLUMNS : int { ID = 0, NAME, AMOUNT, PRICE, TOTAL, CURRENCY };

static inline std::string getProjectCurrentPath() { return std::filesystem::current_path().string(); }

/**
 * @brief this function returns goods.db database type
 * open or create db to hold goods amount and prices
 * @param path in which path "goods.db" will be created or opened
 * @return SQLite::Database db datatype
 */
SQLite::Database open_goods_database( const std::string& file_name );

class GoodsManager {
 public:
  /**
   * @brief constructor of Goods class
   *
   * @param db database data type
   * @param table_name name of the goods table name which is created in database.
   */
  GoodsManager( SQLite::Database& db, std::string table_name );

  ~GoodsManager();

  /**
   * @brief Add goods to database
   * @param elem data structure of goods that be hold in database.
   */
  void add_goods( const Goods<>& elem ) const;

  /**
   * @brief read goods values from database
   * @param id key id in database
   * @return goods values in terms of GoodsElems
   */
  Goods<> get_goods( int id ) const;

  /**
   * @brief this function reads total number of rows in the database
   * @return the number of rows in the database
   */
  int get_row_number() const;

  /**
   * @brief update goods with id and amound
   * @param id table id number of goods that would be updated
   * @param amount update amount value
   */
  void update_with_id( int id, double amount ) const;

  /**
   * @brief delete goods by id from database
   * @param id number of good that would be deleted
   */
  void delete_with_id( int id ) const;

  /**
   * @brief returns all from database goods in vector
   *
   * @return the vector container of goods from db.
   */
  std::vector<Goods<>> get_all_goods() const;

  /**
   * @brief reads goods values from yahoo finance and create a goodselem object
   * @param symbol it is the symbol of goods which is defined in yahoo finance
   * @param amount amount of goods
   * @return goods obecjt
   */
  Goods<> create_goods( std::string_view symbol, double amount );

  /**
   * @brief calcuates the total wealth in terms of given currency
   * @param currency
   * @return returns total wealth in the given currency
   */
  double calculate_total_wealth( std::string_view currency );

  /**
   * @brief updates prices of goods from yahoo finance
   */
  void update_goods_prices();

  /**
   * @brief insert the last updated date and USD amount of goods.
   *
   */
  void insert_last_updated_amount();

  /**
   * @brief reads updated dates and total amount from db
   * @return vector of pairs of date and total amount
   */
  std::vector<std::pair<std::string, double>> get_updated_dates() const;

 private:
  SQLite::Database& m_Database;  //< Reference to the SQLite Database Connection
  std::string m_TableName;


  /**
   * @brief reads goods value according to its symbol from yahoo finance
   * @param symbol it is symbol of goods which is defined in yahoo finance website
   * @return json object of values
   */
  rapidjson::Document get_goods_values_from_yahoo_finance( std::string_view symbol );
  static size_t WriteCallback( void* contents, size_t size, size_t nmemb, void* userp );
};

}  // namespace GoodsTrack