// File: ceMySQL.h
// Description: Using MySQL Server with Connector C++ 8.0
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

#ifndef CEMYSQL_H
#define CEMYSQL_H

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <mysqlx/xdevapi.h>

class MyRec {
public:
    int id;
    std::string name;
    std::string time;
    MyRec();//c_tor
};

class ceMySQL {
public:
    ceMySQL();
    ceMySQL(std::string constr,std::string dbname,std::string tbname);
    ~ceMySQL();    
    template <typename T>
        std::string ToString(T a);
    void SetDataSource(std::string constr,std::string dbname,std::string tbname);
    MyRec Select(int id);
    void Delete(int id);
    void Update(MyRec r);
    void Insert(MyRec r);
    void Write(MyRec r);
    std::vector<MyRec> SelectAll();
private:
    std::string conn;
    std::string dbase;
    std::string table;
};

#endif //CEMYSQL_H

