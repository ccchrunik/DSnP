/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(): _M(0), _I(0), _L(0), _O(0), _A(0) {}
   ~CirMgr() {}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;

private:
   int _M, _I, _L, _O, _A, _capacity;
   GateList _piList;
   GateList _poList;
   GateList _totalList;
   GateList _dfsList;

   set<size_t> _floatList;
   set<size_t> _unusedList;
   vector<pair<string, size_t>> symbols; 
   vector<pair<string, size_t>> comments;

   bool checkErr(GateType, string&);
   bool readHeader(ifstream&, vector<string>&);
   bool readPI(ifstream&, int&, size_t&);
   bool readPO(ifstream&, int&, size_t&);
   bool readAIG(ifstream&, int&, size_t&);
   bool read_Sym_Com(ifstream&, size_t&);

   bool connect();
   bool connect_gate(int&, GateType);

   bool genDFSlist();
   void dfs_search(CirGate*);

   void genFloatingList();

   void AIG_Print_Fanin(size_t, int, ostream&) const;

   void writeGate(CirGate*, bool, ostream&) const;


};

#endif // CIR_MGR_H
