/////////////////////////////////////////////////////////////////////////////
// Name:        jtest.cpp
// Purpose:     Testing jsoncpp
// Author:      Yan Naing Aye
// Created:     25/03/2020
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <sstream>
#include <json/json.h>

using namespace std;

int main() {
    string document(("{\"Serial\" : {\"Baud\":9600, \"Parity\":\"Even\"}}"));
    stringstream ss;
    ss.str(document);
    // Json::Reader reader;
    Json::CharReaderBuilder reader;
    Json::Value obj;
    string errs;
    // reader.parse(document,obj);
    if(Json::parseFromStream(reader,ss,&obj,&errs)){
        cout << "Serial: baudrate = " << obj["Serial"]["Baud"].asUInt();
        cout << " parity = " << obj["Serial"]["Parity"].asString()<<endl;
    }    
    return 0;
}