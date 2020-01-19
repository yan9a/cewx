#include "ceODBC.h"
main()
{
  string rs;
  vector< vector<string> > td;
  
  ceODBC db; // initialize
  db.Connect("mssqlserver","sa","password"); // connect
  
  if(db.IsConnected()==1) printf("Connected \n");
  else printf("Disconnected \n");
  
  td = db.SQL("SELECT * FROM myTable"); // exe sql 
  
  // display
  for(int i=0;i<td.size();i++){
    rs="";
    for(int j=0;j<td[i].size();j++){
      rs+=td[i][j]+" ";
    }
    printf("%s \n",rs.c_str());
  }
  
  db.Disconnect(); // disconnect
}
