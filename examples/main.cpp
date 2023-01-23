#include <iostream>
#include <utility>
#include "menu_utility.h"

using namespace GoodsTrack;

int main() {
  std::cout << "GCC version: " << __VERSION__ << std::endl;

  std::string db_name { "goods.db" };
  auto db = open_goods_database( db_name );
  GoodsManager obj( db, "goods" );

  auto pass { false };
  while ( true ) {
    try {
      if ( !pass ) {
        print_good_db( obj );
        pass = false;
      }

      display_db_selection();
      int choice {};
      std::cin >> choice;

      switch ( choice ) {
        case 1: {
          std::cout << "Please, enter the symbol of goods: ";
          std::string goods_symb;
          std::cin >> goods_symb;
          std::transform( goods_symb.begin(), goods_symb.end(), goods_symb.begin(), ::toupper );

          std::cout << "Please, enter the amount for " << goods_symb << " : ";
          double goods_amount {};
          std::cin >> goods_amount;
          obj.add_goods( obj.create_goods( goods_symb, goods_amount ) );
          std::cout << BOLD( FGRN( "Added to " << db_name << " database\n" ) );
          break;
        }
        case 2: {
          std::cout << "Please, enter the id of goods to update: ";
          int goods_id {};
          std::cin >> goods_id;

          std::cout << "Please, enter the amount for " << goods_id << " : ";
          double goods_amount {};
          std::cin >> goods_amount;
          obj.update_with_id( goods_id, goods_amount );
          std::cout << BOLD( FGRN( "Update id " << goods_id << " in database\n" ) );
          break;
        }
        case 3: {
          std::cout << "Please, enter the id of goods to delete: ";
          int goods_id {};
          std::cin >> goods_id;

          obj.delete_with_id( goods_id );
          std::cout << BOLD( FGRN( "Deleted id " << goods_id << " in database\n" ) );
          break;
        }
        case 4: {
          obj.update_goods_prices();
          std::cout << BOLD( FGRN( "Update " << db_name << " successfully\n" ) );
          break;
        }

        case 5: {
          std::cout << "Please, enter the symbol of currency: ";
          std::string goods_symb;
          std::cin >> goods_symb;
          std::transform( goods_symb.begin(), goods_symb.end(), goods_symb.begin(), ::toupper );

          print_good_db( obj, goods_symb );
          pass = true;
          break;
        }
        case 6:
          std::cout << BOLD( FGRN( "Application is closing...\n" ) );
          return 0;
        default:
          std::cout << UNDL( FRED( "\nInvalid choice. Please try again\n" ) );
          std::cin.clear();
          std::cin.ignore( 1, '\n' );
          break;
      }

    } catch ( const std::exception& e ) {
      std::cout << BOLD( FRED( "SQLite exception: " << e.what() << "\n" ) );
      std::cout << BOLD( FRED( "Please, try again!!!\n" ) );
    }
  }
}
