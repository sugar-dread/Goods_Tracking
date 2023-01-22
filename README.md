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


sudo apt-get install libsqlite3-dev

sudo apt-get install rapidjson-dev

sudo apt-get install libcurl4-openssl-dev