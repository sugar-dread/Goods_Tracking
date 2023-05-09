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

constexpr int column_size = 6;
constexpr int indent_wid = 20;
constexpr int id_wid = 10;
constexpr int name_wid = 15;
constexpr int amount_wid = name_wid;
constexpr int price_wid = name_wid;
constexpr int tot_wid = name_wid;
constexpr int currency_wid = name_wid;
constexpr int total_wid = id_wid + name_wid + amount_wid + price_wid + tot_wid + currency_wid + column_size;

using table_t = std::array<std::string, column_size>;
const table_t headers { { "id", "name", "amount", "price", "total", "currency" } };

template <typename T = char>
void print_break( T c = '-' ) {
  std::cout << std::setw( total_wid + 1 );
  std::cout << std::setfill( c );
  std::cout << '\n';
  std::cout << std::setfill( ' ' );
}

template <typename T = char>
void print_line( table_t const& tbl, T c = '|' ) {
  auto const& [id, name, amount, price, total, currency] = tbl;

  std::cout << c << std::setw( id_wid ) << id;
  std::cout << c << std::setw( name_wid ) << name;
  std::cout << c << std::setw( amount_wid ) << amount;
  std::cout << c << std::setw( price_wid ) << price;
  std::cout << c << std::setw( tot_wid ) << total;
  std::cout << c << std::setw( currency_wid - 1 ) << currency;
  std::cout << c << '\n';
}

template <typename T = char>
void print_line_db( const Goods& goods, T c = '|' ) {
  std::cout << std::left << c << std::setw( id_wid ) << goods.getId();
  std::cout << std::right << c << std::setw( name_wid ) << goods.getName();
  std::cout << std::right << c << std::setw( amount_wid ) << goods.getAmount();
  std::cout << std::right << c << std::setw( price_wid ) << goods.getPrice();
  std::cout << std::right << c << std::setw( tot_wid ) << goods.getTotal();
  std::cout << std::right << c << std::setw( currency_wid - 1 ) << goods.getCurrency();

  std::cout << c << "\n";
}

template <typename T = char>
void print_total_wealth( GoodsManager& obj, std::string_view currency, T c = '|' ) {
  std::locale::global( std::locale( "en_US.UTF-8" ) );
  std::cout.imbue( std::locale() );
  auto total_val = obj.calculate_total_wealth( currency.data() );

  std::stringstream stmt;
  stmt << c << "Total Wealth (" << currency.data() << ") : " << std::fixed << std::setprecision( 2 ) << total_val;
  std::cout << stmt.str();
  std::cout.width( total_wid - stmt.str().size() );
  std::cout << c << "\n";
}

void print_good_db( GoodsManager& obj, std::string_view currency = "" ) {
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
  std::cout << " 5. Print updated date and amount\n";
  std::cout << " 6. Show with specific currency \n";
  std::cout << " 7. Exit\n";
  std::cout << BOLD( FGRN( ">> " ) );
}
}  // namespace GoodsTrack