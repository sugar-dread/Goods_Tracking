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
constexpr char endpoint_1[] = "https://query1.finance.yahoo.com/v11/finance/quoteSummary/";
constexpr char endpoint_2[] = "?modules=price";

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
  std::pair<double, std::string> operator()( std::string_view goodsSymbol );

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

/*
https://stackoverflow.com/questions/44030983/yahoo-finance-url-not-working

Yahoo has gone to a Reactjs front end which means if you analyze the request headers from the client to the backend you can get the actual JSON they use to populate the client side stores.

Hosts:
query1.finance.yahoo.com HTTP/1.0
query2.finance.yahoo.com HTTP/1.1 (difference between HTTP/1.0 & HTTP/1.1)
If you plan to use a proxy or persistent connections use query2.finance.yahoo.com. But for the purposes of this post, the host used for the example URLs is not meant to imply anything about the path
it's being used with.

Fundamental Data
(substitute your symbol for: AAPL)

/v10/finance/quoteSummary/AAPL?modules=
Inputs for the ?modules= query:

    [
       'assetProfile',
       'summaryProfile',
       'summaryDetail',
       'esgScores',
       'price',
       'incomeStatementHistory',
       'incomeStatementHistoryQuarterly',
       'balanceSheetHistory',
       'balanceSheetHistoryQuarterly',
       'cashflowStatementHistory',
       'cashflowStatementHistoryQuarterly',
       'defaultKeyStatistics',
       'financialData',
       'calendarEvents',
       'secFilings',
       'recommendationTrend',
       'upgradeDowngradeHistory',
       'institutionOwnership',
       'fundOwnership',
       'majorDirectHolders',
       'majorHoldersBreakdown',
       'insiderTransactions',
       'insiderHolders',
       'netSharePurchaseActivity',
       'earnings',
       'earningsHistory',
       'earningsTrend',
       'industryTrend',
       'indexTrend',
       'sectorTrend']


{
"quoteSummary":{
"result":[
   {
      "price":{
         "maxAge":1,
         "preMarketChange":{

         },
         "preMarketPrice":{

         },
         "preMarketSource":"FREE_REALTIME",
         "postMarketChangePercent":{
            "raw":0.00392712,
            "fmt":"0.39%"
         },
         "postMarketChange":{
            "raw":0.0290999,
            "fmt":"0.03"
         },
         "postMarketTime":1683331196,
         "postMarketPrice":{
            "raw":7.4391,
            "fmt":"7.44"
         },
         "postMarketSource":"FREE_REALTIME",
         "regularMarketChangePercent":{
            "raw":0.004065,
            "fmt":"0.41%"
         },
         "regularMarketChange":{
            "raw":0.0299997,
            "fmt":"0.03"
         },
         "regularMarketTime":1683316802,
         "priceHint":{
            "raw":2,
            "fmt":"2",
            "longFmt":"2"
         },
         "regularMarketPrice":{
            "raw":7.41,
            "fmt":"7.41"
         },
         "regularMarketDayHigh":{
            "raw":7.5499,
            "fmt":"7.55"
         },
         "regularMarketDayLow":{
            "raw":7.32,
            "fmt":"7.32"
         },
         "regularMarketVolume":{
            "raw":45848838,
            "fmt":"45.85M",
            "longFmt":"45,848,838.00"
         },
         "averageDailyVolume10Day":{

         },
         "averageDailyVolume3Month":{

         },
         "regularMarketPreviousClose":{
            "raw":7.38,
            "fmt":"7.38"
         },
         "regularMarketSource":"FREE_REALTIME",
         "regularMarketOpen":{
            "raw":7.44,
            "fmt":"7.44"
         },
         "strikePrice":{

         },
         "openInterest":{

         },
         "exchange":"NYQ",
         "exchangeName":"NYSE",
         "exchangeDataDelayedBy":0,
         "marketState":"CLOSED",
         "quoteType":"EQUITY",
         "symbol":"PLTR",
         "underlyingSymbol":null,
         "shortName":"Palantir Technologies Inc.",
         "longName":"Palantir Technologies Inc.",
         "currency":"USD",
         "quoteSourceName":"Nasdaq Real Time Price",
         "currencySymbol":"$",
         "fromCurrency":null,
         "toCurrency":null,
         "lastMarket":null,
         "volume24Hr":{

         },
         "volumeAllCurrencies":{

         },
         "circulatingSupply":{

         },
         "marketCap":{
            "raw":15694157824,
            "fmt":"15.69B",
            "longFmt":"15,694,157,824.00"
         }
      }
   }
],
"error":null
}
}*/