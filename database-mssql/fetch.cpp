#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
#include <string.h>

main() {
  SQLHENV env; //environment handle
  SQLHDBC dbc; //connection handle
  SQLHSTMT stmt; // statement handle
  SQLRETURN ret; //return status

  SQLCHAR outstr[1024]; // output string
  SQLSMALLINT outstrlen; // output string length

  SQLSMALLINT columns; // number of columns
  int row = 0;


  SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env); //allocate environment handle
  SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0); // ODBC version 3
  SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc); // allocate connection handle

  // Connect to a DSN
  ret = SQLDriverConnect(dbc, NULL, 
    (SQLCHAR*)"DSN=mssqlserver;UID=sa;PWD=password", SQL_NTS,
    outstr, sizeof(outstr), &outstrlen,
    SQL_DRIVER_NOPROMPT);
  if (SQL_SUCCEEDED(ret)) {
    printf("Connected\n");
    if (ret == SQL_SUCCESS_WITH_INFO) {
      printf("Driver reported diagnostics\n");
    }

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt); // allocate statement handle
    ret=SQLExecDirect(stmt, (SQLCHAR*)"select * from myTable", SQL_NTS);

    if (SQL_SUCCEEDED(ret)) {
        SQLNumResultCols(stmt, &columns);//get numbers of columns
        printf("Number of columns: %d",columns);

        //Loop through the rows
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            SQLUSMALLINT i;
            printf("Row %d\n", row++);
            // Loop through the columns
            for (i = 1; i <= columns; i++) {
                SQLLEN indicator;
                char buf[512];
                // retrieve column data as a string
                ret = SQLGetData(stmt, i, SQL_C_CHAR,
                  buf, sizeof(buf), &indicator);
                if (SQL_SUCCEEDED(ret)) {
                    // Handle null columns
                    if (indicator == SQL_NULL_DATA) strcpy(buf, "NULL");
                    printf("  Column %u : %s\n", i, buf);
                }
            }
        }
    }
    else{
        printf("Error in SQLExecDirect");
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    SQLDisconnect(dbc); // disconnect
  } 
  else {
    printf("Failed to connect\n");
  }
  
  // free up allocated handles
  SQLFreeHandle(SQL_HANDLE_DBC, dbc);
  SQLFreeHandle(SQL_HANDLE_ENV, env);
}
