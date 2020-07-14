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
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr(): _M(0), _I(0), _L(0), _O(0), _A(0) {}
   ~CirMgr() {} 

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { return _totalList[gid]; }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

private:
   ofstream           *_simLog;
   int _M, _I, _L, _O, _A, _capacity;
   GateList _piList;
   GateList _poList;
   GateList _totalList;
   GateList _dfsList;

   set<size_t> _floatList;
   set<size_t> _unusedList;
   set<size_t> _pi;
   set<size_t> _po;   
   vector<pair<string, size_t>> symbols; 
   vector<pair<string, size_t>> comments;

   vector<vector<GateV> > FEC_GROUPS;

   bool checkErr(GateType, string&);
   bool readHeader(ifstream&, vector<string>&);
   bool readPI(ifstream&, int&, size_t&);
   bool readPO(ifstream&, int&, size_t&);
   bool readAIG(ifstream&, int&, size_t&);
   bool read_Sym_Com(ifstream&, size_t&);
   void reCount();

   bool connect();
   bool connect_gate(int&, GateType);

   bool genDFSlist();
   void genPIList();
   void genPOList();
   void dfs_search(CirGate*);

   void genFloatingList();
   // sweeping function
   void sweepGate(CirGate*, set<size_t>&);
   // optimize function
   void clear_fanin_gate(CirGate*, CirGate*);
   void clear_fanout_gate(CirGate*, CirGate*);
   // to cut out a specific gate
   void replace_fanout_opt(CirGate*, CirGate*, bool); 
   void replace_fanin_opt(CirGate*, CirGate*, bool);
   // ---------strash function-----------
   // to replace a specific gate
   void st_merge(CirGate*, CirGate*);
   bool sth_check(HashKey, HashKey);
   void replace_fanout_sth(CirGate*, CirGate*);
   void replace_fanin_sth(CirGate*, CirGate*);
   // ----------simulation-----------
   void simulate();
   void genOutputPattern(vector<string>&);
   void writeSimFile(vector<string>&, vector<string>&);
   // -----------fraig------------
   void replace_fanout_frg(GateV&, GateV&);
   void replace_fanin_frg(GateV&, GateV&);
   void genProofModel(SatSolver&);
   void reportResult(const SatSolver&, bool);
   void fraig_solver(SatSolver&, vector<GateV>&);

   void writeGate(CirGate*, bool, ostream&) const;
   void AIG_Print_Fanin(size_t, int, ostream&) const;

};

#endif // CIR_MGR_H
