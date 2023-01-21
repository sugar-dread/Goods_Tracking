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

#include <filesystem>
#include "SQLiteCpp.h"

namespace GoodsTrack {

static inline std::string getProjectCurrentPath() { return std::filesystem::current_path().string(); }

/**
 * @brief this function returns goods.db database type
 * open or create db to hold goods amount and prices
 * @param path in which path "goods.db" will be created or opened
 * @return SQLite::Database db datatype
 */
SQLite::Database open_goods_database( const std::string& file_name );

class GoodsElems {
 public:
  void setId( int id ) { m_Id = id; }
  int getId() const { return m_Id; }

  void setName( std::string_view name ) { m_Name = name.data(); }
  std::string getName() const { return m_Name; }

  void setAmount( double amount ) { m_Amount = amount; }
  double getAmount() const { return m_Amount; }

  void setPrice( double price ) { m_Price = price; }
  double getPrice() const { return m_Price; }

  void setTotal( double total ) { m_Total = total; }
  double getTotal() const { return m_Total; }

  void setCurrency( std::string_view currency ) { m_Currency = currency.data(); }
  std::string getCurrency() const { return m_Currency; }

 public:
  int m_Id {};
  std::string m_Name {};
  double m_Amount {};
  double m_Price {};
  double m_Total {};
  std::string m_Currency {};
};

class GoodsManager {
 public:
  /**
   * @brief constructor of Goods class
   *
   * @param db database data type
   * @param table_name name of the goods table name which is created in database.
   */
  GoodsManager( SQLite::Database& db, std::string table_name ) : m_Database { db }, m_TableName { std::move( table_name ) } {}

  //~GoodsManager();

  /**
   * @brief Add goods to database
   *
   * @param elem data structure of goods that be hold in database.
   */
  void add_goods( const GoodsElems& elem ) const;

  /**
   * @brief read goods values from database
   * @param id key id in database
   * @return goods values in terms of GoodsElems
   */
  GoodsElems get_goods( int id ) const;

 private:
  SQLite::Database& m_Database;  //< Reference to the SQLite Database Connection
  std::string m_TableName;
};

}  // namespace GoodsTrack