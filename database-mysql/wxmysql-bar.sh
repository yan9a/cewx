g++ wxmysql.cpp ceMySQL.cpp -std=c++11 \
    `wx-config --cxxflags --libs` \
    -I /usr/local/mysql/connector-c++-8.0/include \
    -L /usr/local/mysql/connector-c++-8.0/lib64 \
    -lmysqlcppconn8 -o wxmysql
./wxmysql