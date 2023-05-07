#include "webconn.h"
#include <rapidjson/document.h>
//#include <rapidjson/error/en.h>
//#include <rapidjson/filereadstream.h>
//#include <rapidjson/istreamwrapper.h>
//#include <rapidjson/prettywriter.h>
//#include <rapidjson/rapidjson.h>BUILD_TESTS
//#include <rapidjson/schema.h>
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

double WebConn::operator()( std::string_view goodsSymbol ) {
  const std::string endpoint = "https://query1.finance.yahoo.com/v11/finance/quoteSummary/" + std::string { goodsSymbol.data() } + "?modules=price";
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

  if ( doc.HasMember( "quoteSummary" ) && doc["quoteSummary"].IsObject() && doc["quoteSummary"].HasMember( "result" ) && doc["quoteSummary"]["result"].IsArray() &&
       doc["quoteSummary"]["result"].Size() > 0 && doc["quoteSummary"]["result"][0].IsObject() && doc["quoteSummary"]["result"][0].HasMember( "price" ) &&
       doc["quoteSummary"]["result"][0]["price"].IsObject() && doc["quoteSummary"]["result"][0]["price"].HasMember( "regularMarketPrice" ) &&
       doc["quoteSummary"]["result"][0]["price"]["regularMarketPrice"].IsObject() && doc["quoteSummary"]["result"][0]["price"]["regularMarketPrice"].HasMember( "raw" ) &&
       doc["quoteSummary"]["result"][0]["price"]["regularMarketPrice"]["raw"].IsDouble() ) {
    return doc["quoteSummary"]["result"][0]["price"]["regularMarketPrice"]["raw"].GetDouble();
  } else {
    throw SQLite::Exception( "there is error on price JSON : " + std::string( goodsSymbol.data() ) );
  }
}

};  // namespace GoodsTrack