g++ -std=c++11 -I /usr/local/mysql/connector-c++-8.0/include -I /usr/local/boost_1_66_0 -L /usr/local/mysql/connector-c++-8.0/lib jdbc_test.cpp -lmysqlcppconn -o jdbc_test
./jdbc_test
