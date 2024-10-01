#include "webconn.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <iostream>

namespace GoodsTrack {

WebConn::WebConn() {
  curl_global_init( CURL_GLOBAL_ALL );

  if ( m_Curl = curl_easy_init(); !m_Curl ) {
    throw SQLite::Exception( "failed to init curl" );
  }
}

WebConn::~WebConn() {
  curl_easy_cleanup( m_Curl );
  curl_global_cleanup();
}

size_t WebConn::WriteCallback( void* contents, size_t size, size_t nmemb, void* userp ) {
  ( (std::string*)userp )->append( (char*)contents, size * nmemb );
  return size * nmemb;
}

std::pair<double, std::string> WebConn::operator()( std::string_view goodsSymbol, std::string_view apiKey ) {
  const std::string endpoint = endpoint_1 + std::string { goodsSymbol.data() } + "&apikey=" + std::string { apiKey.data() };
  std::string response;

  curl_easy_setopt( m_Curl, CURLOPT_URL, endpoint.c_str() );
  curl_easy_setopt( m_Curl, CURLOPT_WRITEFUNCTION, WriteCallback );
  curl_easy_setopt( m_Curl, CURLOPT_WRITEDATA, &response );

  if ( CURLcode res = curl_easy_perform( m_Curl ); res != CURLE_OK ) {
    curl_easy_cleanup( m_Curl );
    curl_global_cleanup();
    throw SQLite::Exception( "Error sending HTTP request: " + std::string( curl_easy_strerror( res ) ) );
  }

  char* content_type { nullptr };
  curl_easy_getinfo( m_Curl, CURLINFO_CONTENT_TYPE, &content_type );

  rapidjson::Document doc;
  doc.SetObject();
  doc.Parse( response.c_str() );
  std::cout << goodsSymbol.data() << '\n';

  if ( doc.HasParseError() && !doc.HasMember( "Global Quote" ) && !doc["Global Quote"].HasMember( "05. price" ) ) {
    throw SQLite::Exception( "there is no valid json file for " + std::string( goodsSymbol.data() ) );
  }

  double price = std::stod( doc["Global Quote"]["05. price"].GetString() );
  return std::make_pair( price, "USD" );
}

};  // namespace GoodsTrack