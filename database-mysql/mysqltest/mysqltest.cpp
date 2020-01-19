#include <iostream>
#include <mysqlx/xdevapi.h>
using namespace std;
using namespace mysqlx;

int main()
try {
  cout <<"Getting session..." <<endl;
  Session sess("root:password@127.0.0.1:33060");
  //Session sess("localhost",33060,"root","password");
  cout <<"Session accepted, getting schemas list ..." <<endl;
  
  //Get a list of all available schemas
  std::list<Schema> schemaList = sess.getSchemas();
  cout<<"Available schemas in this session:"<< endl;

  //loop over all available schemas and print their name
  for(Schema schema : schemaList) {
    cout << schema.getName() <<endl;
  }
}
catch (const mysqlx::Error &err)
{
  cout << "ERROR: " << err << endl;
}
