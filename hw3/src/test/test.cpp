/****************************************************************************
  FileName     [ test.cpp ]
  PackageName  [ test ]
  Synopsis     [ Test program for simple database db ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include "dbJson.h"

using namespace std;

extern DBJson dbjson;

class CmdParser;
CmdParser* cmdMgr = 0; // for linking purpose

int
main(int argc, char** argv)
{
   if (argc != 2) {  // testdb <jsonfile>
      cerr << "Error: using testdb <jsonfile>!!" << endl;
      exit(-1);
   }

   ifstream inf(argv[1]);

   if (!inf) {
      cerr << "Error: cannot open file \"" << argv[1] << "\"!!\n";
      exit(-1);
   }

   if (dbjson) {
      cout << "Table is resetting..." << endl;
      dbjson.reset();
   }
   // if (!(inf >> dbjson)) {
   //    cerr << "Error in reading JSON file!!" << endl;
   //    exit(-1);
   // }
   inf >> dbjson;
   size_t idx;
   cout << "========================" << endl;
   cout << " Print JSON object" << endl;
   cout << "========================" << endl;
   DBJsonElem a("Eric", 50);
   dbjson.add(a);
   DBSortKey key;
   DBSortValue value;
   dbjson.sort(key);
   cout << dbjson << endl;
   cout << dbjson.max(idx) << ' ' << idx << endl;
   cout << dbjson.min(idx) << ' ' << idx << endl;
   dbjson.sort(value);
   cout << dbjson << endl;
   cout << dbjson.max(idx) << ' ' << idx << endl;
   cout << dbjson.min(idx) << ' ' << idx << endl;
   cout << dbjson.sum() << endl;
   cout << fixed << setprecision(1) << dbjson.ave() << endl;
   // TODO
   // Insert what you want to test here by calling DBJson's member functions

   return 0;
}
