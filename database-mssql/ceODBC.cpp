// File: ceODBC.cpp
// Description: Using MS SQL Server with ODBC
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

// References
// https://www.easysoft.com/developer/languages/c/odbc_tutorial.html

#include "ceODBC.h"

// constructor
// allocate environment handle and connection handle
ceODBC::ceODBC()
{
  SQLRETURN ret; //return status
  NumberOfRowsAffected=0;
  try{
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env); //allocate environment handle
    if (!SQL_SUCCEEDED(ret)) {
      GetError("SQLAllocHandle", env, SQL_HANDLE_ENV);
      return;
    }
    SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0); // ODBC version 3
    ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc); // allocate connection handle
    if (!SQL_SUCCEEDED(ret)) {
      GetError("SQLAllocHandle", dbc, SQL_HANDLE_DBC);
      return;
    }   
  }
  catch(...){
    printf("Error in constructor\n");
  }  
}

// destructor
// free up connection handle and environment handle
ceODBC::~ceODBC()
{
    // free up allocated handles
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    SQLFreeHandle(SQL_HANDLE_ENV, env);    
}

// connect to datasource
// inputs: dsn, username, password
int ceODBC::Connect(string dsn, string username, string password)
{
  SQLRETURN ret; //return status
  SQLCHAR outstr[1024]; // output string
  SQLSMALLINT outstrlen; // output string length
  
  try{
    // Connect to a DSN
    string constr="DSN="+dsn+";UID="+username+";PWD="+password;
    ret = SQLDriverConnect(dbc, NULL, 
      (SQLCHAR*)constr.c_str(), SQL_NTS,
      outstr, sizeof(outstr), &outstrlen,
      SQL_DRIVER_NOPROMPT);
    if (!SQL_SUCCEEDED(ret)) {
      GetError("SQLDriverConnect", dbc, SQL_HANDLE_DBC);
      return -1;
    }
    if (ret == SQL_SUCCESS_WITH_INFO) {
      GetError("SQLDriverConnect", dbc, SQL_HANDLE_DBC);
    }
  }
  catch(...){
    printf("Connect error\n");
    return -1;
  }
  return 0;  
}

vector< vector<string> > ceODBC::SQL(string statement) {    
  SQLRETURN ret; //return status  
  vector< vector<string> > ds; // return dataset
  SQLSMALLINT columns; // number of columns
  SQLLEN rows; // number of rows
  try{
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt); // allocate statement handle
    if (!SQL_SUCCEEDED(ret)) {
      GetError("SQLAllocHandle", stmt, SQL_HANDLE_STMT);
      return ds;
    }
    ret = SQLExecDirect(stmt, (SQLCHAR*)statement.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
      GetError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
      return ds;
    }    
    SQLNumResultCols(stmt, &columns);//get numbers of columns
    SQLRowCount(stmt, &rows); // get number of rows affected for UPDATE, INSERT, DELETE statements
    //printf("Number of rows affected: %ld \n",(long int)rows);
    NumberOfRowsAffected=(long int)rows;
    
    //Loop through the rows
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
        SQLUSMALLINT i;
        vector<string> rd; // create an empty row
        string tstr;
        // Loop through the columns
        for (i = 1; i <= columns; i++) {
            SQLLEN indicator;
            char buf[512];           
            // retrieve column data as a string
            ret = SQLGetData(stmt, i, SQL_C_CHAR, buf, sizeof(buf), &indicator);
            if (SQL_SUCCEEDED(ret)) {
                tstr=buf;
                if (indicator == SQL_NULL_DATA) tstr="";                        
                rd.push_back(tstr);
            }            
        }
        ds.push_back(rd);
    }    
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);    
  }
  catch(...){
    printf("Error in SQL method\n");
  }
}

int ceODBC::Disconnect()
{
  SQLRETURN ret; //return status
  try{
    SQLDisconnect(dbc); // disconnect    
  }
  catch(...){
    printf("Disconnect error\n");
    return -1;
  }
  return 0;
}

vector<string> ceODBC::GetError(char const *fn,SQLHANDLE handle,SQLSMALLINT type)
{
    SQLINTEGER   i = 0;
    SQLINTEGER   native;
    SQLCHAR      state[7];
    SQLCHAR      text[256];
    SQLSMALLINT  len;
    SQLRETURN    ret;
    vector<string> emes;
    emes.push_back(fn);
    do { 
        ret = SQLGetDiagRec(type, handle, ++i, state, &native, text,sizeof(text), &len);
        if (SQL_SUCCEEDED(ret)) { 
          printf("%s:%ld:%ld:%s\n", state, (long int)i, (long int)native, text); 
          emes.push_back((char*)text);
        }            
    } while( ret == SQL_SUCCESS );
    return emes;
}

// check connection status
// 1 : connected
// 0 : disconnected
// -1: error, connection doesn't exist
int ceODBC::IsConnected()
{
  SQLRETURN    ret;
  SQLUINTEGER	uIntVal; // Unsigned int attribute values
  ret = SQLGetConnectAttr(dbc,SQL_ATTR_CONNECTION_DEAD,(SQLPOINTER)&uIntVal,(SQLINTEGER) sizeof(uIntVal),NULL);
  if (!SQL_SUCCEEDED(ret)) {
    GetError("SQLGetConnectAttr(SQL_ATTR_CONNECTION_DEAD)", dbc, SQL_HANDLE_DBC);
    return -1;
  }
  if (uIntVal==SQL_CD_FALSE) return 1; // The connection is open and available for statement processing.
  if (uIntVal==SQL_CD_TRUE) return 0; // The connection to the server has been lost.
  
  return -1;
}
