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

std::pair<double, std::string> WebConn::operator()( std::string_view goodsSymbol ) {
  const std::string endpoint = std::string { endpoint_1 } + std::string { goodsSymbol.data() };
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

  if ( doc.Parse( response.c_str() ).HasParseError() ) {
    throw SQLite::Exception( "there is no valid json file for " + std::string( goodsSymbol.data() ) );
  }

  double price { 0.0 };
  std::string currency {};

  if ( doc.HasMember( "chart" ) && doc["chart"].IsObject() && doc["chart"].HasMember( "result" ) && doc["chart"]["result"].IsArray() && doc["chart"]["result"].Size() > 0 &&
       doc["chart"]["result"][0].IsObject() && doc["chart"]["result"][0].HasMember( "meta" ) && doc["chart"]["result"][0]["meta"].IsObject() &&
       doc["chart"]["result"][0]["meta"].HasMember( "regularMarketPrice" ) && doc["chart"]["result"][0]["meta"]["regularMarketPrice"].IsDouble() ) {
    price = doc["chart"]["result"][0]["meta"]["regularMarketPrice"].GetDouble();
    currency = doc["chart"]["result"][0]["meta"]["currency"].GetString();
  } else {
    throw SQLite::Exception( "there is error on price JSON : " + std::string( goodsSymbol.data() ) );
  }

  return std::make_pair( price, currency );
}

};  // namespace GoodsTrack