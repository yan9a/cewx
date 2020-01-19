// File: ceMySQL.cpp
// Description: Using MySQL Server with Connector C++ 8.0
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

#include "ceMySQL.h"
using namespace mysqlx;

MyRec::MyRec()
{
    id=0;
    name="";
    time="";
}
ceMySQL::ceMySQL()
{

}

ceMySQL::ceMySQL(std::string constr,std::string dbname,std::string tbname)
{
    SetDataSource(constr,dbname,tbname);
}

void ceMySQL::SetDataSource(std::string constr,std::string dbname,std::string tbname)
{
    conn=constr;
    dbase=dbname;
    table=tbname;
}

ceMySQL::~ceMySQL()
{
    
}

//example usage
// string str=ToString(num);
template <typename T>
    std::string ceMySQL::ToString(T a)
    {
        std::ostringstream ss;
        ss << a;
        return ss.str();
    }

MyRec ceMySQL::Select(int id)
{
    MyRec r;    
    try {
        Session sess(conn.c_str());
        Schema myDb = sess.getSchema(dbase.c_str());
        Table myTable = myDb.getTable(table.c_str());
        //find
        RowResult myResult = myTable.select("id","name","DATE_FORMAT(time,'%Y-%m-%d %T')")
            .where("id = : pname").bind("pname",id).execute();
        //use "DATE_FORMAT(time,'%Y-%m-%d %T.%f')" to include microseconds
        // get result
        Row row = myResult.fetchOne();
        if(row){
            r.id=row[0];             
            r.name=ToString(row[1]);             
            r.time=ToString(row[2]);             
        }
        else{ //if no record found
            printf("No rec found \n");
        }        
        sess.close();      
    }
    catch (const mysqlx::Error &err){
        printf("Error in ceMySQL Select \n");
    } 
    return r;
}

void ceMySQL::Delete(int id)
{   
    try {
        Session sess(conn.c_str());
        Schema myDb = sess.getSchema(dbase.c_str());
        Table myTable = myDb.getTable(table.c_str());
        //delete a row 
        myTable.remove().where("id = : pname").bind("pname",id).execute();     
        sess.close();      
    }
    catch (const mysqlx::Error &err){
        printf("Error in ceMySQL Delete \n");
    } 
}

void ceMySQL::Update(MyRec r)
{   
    try {
        Session sess(conn.c_str());
        Schema myDb = sess.getSchema(dbase.c_str());
        Table myTable = myDb.getTable(table.c_str());
        //update the record
        myTable.update()
        .set("name",r.name)
        .set("time",r.time)
        .where("id = : pname").bind("pname",r.id).execute();     
        sess.close();      
    }
    catch (const mysqlx::Error &err){
        printf("Error in ceMySQL Update \n");
    } 
}

void ceMySQL::Insert(MyRec r)
{   
    try {
        Session sess(conn.c_str());
        Schema myDb = sess.getSchema(dbase.c_str());
        Table myTable = myDb.getTable(table.c_str());
        //insert a row 
        myTable.insert("id","name","time")
        .values(r.id,r.name,r.time).execute();     
        sess.close();      
    }
    catch (const mysqlx::Error &err){
        printf("Error in ceMySQL Insert \n");
    } 
}

// Write - check id first, 
// if id exists, update 
// else, insert
void ceMySQL::Write(MyRec r)
{   
    try {
        Session sess(conn.c_str());
        Schema myDb = sess.getSchema(dbase.c_str());
        Table myTable = myDb.getTable(table.c_str());
        //find
        RowResult myResult = myTable.select("id","name","DATE_FORMAT(time,'%Y-%m-%d %T')")
            .where("id = : pname").bind("pname",r.id).execute();
        // get result
        Row row = myResult.fetchOne();
        if(row){
            //update the record
            myTable.update()
            .set("name",r.name)
            .set("time",r.time)
            .where("id = : pname").bind("pname",r.id).execute();             
        }
        else{ //if no record found
            //insert a row 
            myTable.insert("id","name","time")
            .values(r.id,r.name,r.time).execute();
        }                     
        sess.close();      
    }
    catch (const mysqlx::Error &err){
        printf("Error in ceMySQL Write \n");
    } 
}

std::vector<MyRec> ceMySQL::SelectAll()
{
    std::vector<MyRec> ar;
    MyRec r;    
    try {
        Session sess(conn.c_str());
        Schema myDb = sess.getSchema(dbase.c_str());
        Table myTable = myDb.getTable(table.c_str());
        //find
        RowResult myResult = myTable.select("id","name","DATE_FORMAT(time,'%Y-%m-%d %T')")
            .execute();
        // get result
        Row row = myResult.fetchOne();
        while(row){
            r.id=row[0];             
            r.name=ToString(row[1]);             
            r.time=ToString(row[2]);   
            ar.push_back(r);
            row=myResult.fetchOne();          
        }       
        sess.close();      
    }
    catch (const mysqlx::Error &err){
        printf("Error in ceMySQL SelectAll \n");
    } 
    return ar;
}