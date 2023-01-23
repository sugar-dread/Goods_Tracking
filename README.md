# Goods_Tracking
This application stores goods like gold, stocks, USD index , EUR index etc in the database. The symbolds of goods should be placed in [https://finance.yahoo.com]. If you would like to add custom goods, you can add it by manual. There is only one database which holds all goods.

The application creates or open goods database when runs. With menu, you can do operations like 'add', 'modify' or 'add_custom' goods.

## Work Environment ##

1. Ubuntu 20.04
2. GCC 11.1.0
3. C++20 standard
4. VSCode version 1.74.3


## SQLite3Cpp
In this API, SQLite3 light database is used. Thanks to Sébastien Rombauts work ([SRombauts](https://github.com/SRombauts)), I used his SQLite3 CPP wrapper library. He explained well how to use it in this [link](https://github.com/SRombauts/SQLiteCpp).

## Required Libraries

SQLiteCpp library wrap C sqlite3 lib. In order to use and compile, C sqlite3 library should be intalled. With dev package, sqlite3 C header can be included for development purporse.

`sudo apt-get install libsqlite3-dev`

Yahoo finance sends back json format response. Rapidjson is a JSON parser library. In order to parse yahoo finance response, rapidjson development package should be installed.

`sudo apt-get install rapidjson-dev`

cURL is a command-line tool and library for transferring data with URLs. It supports various protocols such as HTTP, HTTPS, FTP, SFTP, and more. cURL is commonly used to make HTTP requests from the command line, but it can also be used in programs and scripts, making it a versatile tool for working with web services and APIs. This API uses HTTP request to yahoo finance website.

`sudo apt-get install libcurl4-openssl-dev`