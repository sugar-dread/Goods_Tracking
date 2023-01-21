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
#include <iostream>
#include <sstream>

namespace GoodsTrack {

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

  if ( !exist_flag ) {
    db.exec( "DROP TABLE IF EXISTS goods" );
    db.exec(
        "CREATE TABLE goods (id INTEGER PRIMARY KEY, name TEXT, amount DOUBLE, price DOUBLE, "
        "total DOUBLE, currency TEXT)" );
  }

  return db;
}

void GoodsManager::add_goods( const GoodsElems& elem ) const {
  if ( !( m_Database.tableExists( m_TableName ) ) ) {
    throw SQLite::Exception( "Table " + m_TableName + "does not exists in database" );
  }

  std::stringstream add_stmt;
  add_stmt << "INSERT INTO " + m_TableName + " VALUES " << std::fixed << std::setprecision( 3 ) << "(NULL, "
           << "\"" << elem.getName() << "\", " << elem.getAmount() << ", " << elem.getPrice() << ", " << elem.getTotal() << ", "
           << "\"" << elem.getCurrency() << "\")";

  m_Database.exec( add_stmt.str() );
}

GoodsElems GoodsManager::get_goods( int id ) const {
  if ( !( m_Database.tableExists( m_TableName ) ) ) {
    throw SQLite::Exception( "Table " + m_TableName + "does not exists in database" );
  }

  GoodsElems obj;

  std::stringstream get_goods_by_id;
  get_goods_by_id << "SELECT * FROM " << m_TableName << " WHERE "
                  << "id = " << id;

  SQLite::Statement queryCheck { m_Database, get_goods_by_id.str() };

  auto result { false };
  while ( queryCheck.executeStep() ) {
    if ( id == queryCheck.getColumn( 0 ).getInt() ) {
      result = true;

      obj.setId( queryCheck.getColumn( 0 ).getInt() );
      obj.setName( queryCheck.getColumn( 1 ).getText() );
      obj.setAmount( queryCheck.getColumn( 2 ).getDouble() );
      obj.setPrice( queryCheck.getColumn( 3 ).getDouble() );
      obj.setTotal( queryCheck.getColumn( 4 ).getDouble() );
      obj.setCurrency( queryCheck.getColumn( 5 ).getText() );
    }
  }

  if ( !result ) {
    throw SQLite::Exception( "Goods with id : " + std::to_string( id ) + " does not exists in database" );
  }
  queryCheck.reset();
  return obj;
}

}  // namespace GoodsTrack