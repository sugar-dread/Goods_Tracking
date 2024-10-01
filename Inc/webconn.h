/**
 * @file webConn.h
 * @author your name (you@domain.com)
 * @brief This header file includes webConn class
 * @version 0.1
 * @date 2023-01-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <curl/curl.h>
#include <string>
#include <string_view>
#include <utility>
#include "SQLiteCpp.h"

namespace GoodsTrack {
constexpr char endpoint_1[] = "https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=";

class WebConn {
 public:
  WebConn();

  WebConn( const WebConn& other ) = delete;
  WebConn& operator=( const WebConn& other ) = delete;
  WebConn( WebConn&& other ) = delete;
  WebConn& operator=( WebConn&& other ) = delete;

  /**
   * @brief get the value of the goods
   *
   * @param goodsSymbol Yahoo finance symbol name of the goods
   * @return double price of the goods at that moment in time
   */
  std::pair<double, std::string> operator()( std::string_view goodsSymbol, std::string_view apiKey );

  ~WebConn();

 private:
  CURL* m_Curl { nullptr };

  /**
   * @brief reads goods value according to its symbol from yahoo finance
   * @param symbol it is symbol of goods which is defined in yahoo finance website
   * @return json object of values
   */
  static size_t WriteCallback( void* contents, size_t size, size_t nmemb, void* userp );
};

};  // namespace GoodsTrack
