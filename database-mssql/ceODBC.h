// File: ceODBC.h
// Description: Using MS SQL Server with ODBC
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

#ifndef CEODBC_H
#define CEODBC_H

#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

class ceODBC {
public:
  ceODBC();
  ~ceODBC();
  vector< vector<string> > SQL(string statement);
  int Connect(string dsn, string username, string password);
  int Disconnect();
  int IsConnected();
  long int NumberOfRowsAffected;
private:
  SQLHENV env; //environment handle
  SQLHDBC dbc; //connection handle
  SQLHSTMT stmt; // statement handle
  vector<string> GetError(char const *fn,SQLHANDLE handle,SQLSMALLINT type);
};

#endif //CEODBC_H

