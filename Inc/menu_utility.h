#pragma once

#include <cctype>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <unordered_set>
#include "colorprint.h"
#include "goods_track.h"

namespace GoodsTrack {

constexpr int indent_wid = 20;
constexpr int id_wid = 10;
constexpr int name_wid = 15;
constexpr int amount_wid = name_wid;
constexpr int price_wid = name_wid;
constexpr int tot_wid = name_wid;
constexpr int currency_wid = name_wid;

constexpr int total_wid = id_wid + name_wid + amount_wid + price_wid + tot_wid + currency_wid + column_size;

void print_break() {
  std::cout.width( total_wid );
  std::cout.fill( '-' );
  std::cout << '-' << '\n';
  std::cout.fill( ' ' );
}

void print_line( table_t const& tbl ) {
  auto const& [id, name, amount, price, total, currency] = tbl;

  std::cout << "|";
  std::cout.width( id_wid - 1 );
  std::cout << id << '|';

  std::cout.width( name_wid );
  std::cout << ( ' ' + name ) << '|';

  std::cout.width( amount_wid );
  std::cout << ( ' ' + amount ) << '|';

  std::cout.width( price_wid );
  std::cout << ( ' ' + price ) << '|';

  std::cout.width( tot_wid );
  std::cout << ( ' ' + total ) << '|';

  std::cout.width( currency_wid );
  std::cout << ( ' ' + currency ) << '|';

  std::cout << '\n';
}

void print_line_db( const GoodsElems& goods ) {
  std::cout << std::left << "|" << goods.getId() << std::setw( id_wid - 2 ) << std::setfill( ' ' ) << ' ' << '|';
  std::cout << std::right << std::setw( name_wid ) << goods.getName() << '|';
  std::cout << std::right << std::setw( amount_wid ) << goods.getAmount() << '|';
  std::cout << std::right << std::setw( price_wid ) << goods.getPrice() << '|';
  std::cout << std::right << std::setw( tot_wid ) << goods.getTotal() << '|';
  std::cout << std::right << std::setw( currency_wid ) << goods.getCurrency() << '|';

  std::cout << "\n";
}

void print_total_wealth( const GoodsManager& obj, std::string_view currency ) {
  std::locale::global( std::locale( "en_US.UTF-8" ) );
  std::cout.imbue( std::locale() );
  auto total_val = obj.calculate_total_wealth( currency.data() );

  std::stringstream stmt;
  stmt << "|Total Wealth (" << currency.data() << ") : " << std::fixed << std::setprecision( 2 ) << total_val;
  std::cout << stmt.str();
  std::cout.width( total_wid - stmt.str().size() );
  std::cout << '|';
  std::cout << "\n";
}

void print_good_db( const GoodsManager& obj, std::string_view currency = "" ) {
  print_break();
  print_line( headers );
  print_break();

  for ( auto all_goods_from_db = obj.get_all_goods(); const auto& elem : all_goods_from_db ) {
    print_line_db( elem );
    print_break();
  }

  print_total_wealth( obj, "USD" );
  print_total_wealth( obj, "EUR" );
  print_total_wealth( obj, "TRY" );
  if ( !currency.empty() ) {
    print_total_wealth( obj, currency.data() );
  }

  print_break();
}

void display_db_selection() {
  std::cout << BOLD( FBLU( "Select operation :  \n" ) );
  std::cout << " 1. Add stock \n";
  std::cout << " 2. Update stock \n";
  std::cout << " 3. Delete stock \n";
  std::cout << " 4. Update all stocks \n";
  std::cout << " 5. Show with specific currency \n";
  std::cout << " 6. Exit\n";
  std::cout << BOLD( FGRN( ">> " ) );
}
}  // namespace GoodsTrack