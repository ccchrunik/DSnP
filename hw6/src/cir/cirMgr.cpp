/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <set>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

size_t CirGate::_global_ref = 0;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/

bool 
CirMgr::checkErr(GateType g, string& s)
{
   int count = 0;
   if(g == PI_GATE || g == PO_GATE)
   {
      for(int i = 0; i < s.length(); ++i)
      {
         if(s[i] == ' ')
         {
            parseError(EXTRA_SPACE);
            lineNo = 0; colNo = 0;
            return false;
         }
         else if(s[i] < 48 && s[i] > 57)
         {
            errMsg = s[i];
            parseError(ILLEGAL_IDENTIFIER);
            lineNo = 0; colNo = 0;
            return false;
         }
         ++colNo;
      }
   }

   return true;
}

bool
CirMgr::readHeader(ifstream& f, vector<string>& nums)
{
   string line;
   getline(f, line);
   string num;
   size_t n = myStrGetTok(line, num);
   while (num.size()) 
   {
      nums.push_back(num);
      n = myStrGetTok(line, num, n);
   }
   
   if(nums[0] != "aag")
   {
      cerr << "[ERROR] Line 1: Illegal identifier \"" << nums[0] << "\"!!" << endl;
      return false;
   }

   if(nums.size() < 6)
   {
      cerr << "[ERROR] Line 1: Number of variables is too small(" << nums.size() << ")!!" << endl;
   }
   
   // checkErr(CONST_GATE, line);
   // TODO
   // err condition 2 is not handled
   myStr2Int(nums[1], _M); myStr2Int(nums[2], _I); myStr2Int(nums[3], _L); myStr2Int(nums[4], _O); myStr2Int(nums[5], _A);
   
   return true;
}

bool
CirMgr::readPI(ifstream& f, int& I, size_t& lineNo)
{
   int num;
   string line;
   for(int i = 0; i < I; ++i)
   {
      ++lineNo;
      getline(f, line);
      if(line == "")
      {
         cerr << "[ERROR] Line " << lineNo << ": Missing PI definition!!" << endl;
      }


      myStr2Int(line, num);
      // if(num % 2 != 0)
      // {
      //    cerr << "[ERROR] Line " << lineNo << ": PI index is too big (" << num << ")!!" << endl;
      //    lineNo = 0; colNo = 0;
      //    return false;
      // }
      CirGate* pi = new CirPiGate(size_t(num), lineNo);
      // _piList[num / 2] = pi;
      _totalList[num / 2] = pi;
      // cout << num << endl;
   }

   return true;
}

bool
CirMgr::readPO(ifstream& f, int& O, size_t& lineNo)
{
   int num;
   string line;
   size_t po_index = _M; // po index start with M + 1
   for(int i = 0; i < O; ++i)
   {
      ++lineNo; ++po_index;
      getline(f, line);
      myStr2Int(line, num);
      CirGate* po = new CirPoGate(size_t(num), lineNo, po_index);
      // _poList[po_index] = po;
      _totalList[po_index] = po;
      // cout << num << endl;
   }

   return true;
}

bool
CirMgr::readAIG(ifstream& f, int& A, size_t& lineNo)
{
   string line;
   vector<string> pins;
   string pin;
   int num;
   size_t n;
   
   for(int i = 0; i < A; ++i)
   {
      ++lineNo;
      pins.clear();
      getline(f, line);
      // cout << line << endl;
      // if to check more spaces than expected, we can count the space == 2 and size == 3 is the right case
      n = myStrGetTok(line, pin);
      while (pin.size()) 
      {
         pins.push_back(pin);
         n = myStrGetTok(line, pin, n);
      }
      
      // if(pins.size() != 3) return false; // check for error input

      vector<size_t> ioPins;
      for(int i = 0; i < pins.size(); ++i)
      {
         myStr2Int(pins[i], num);
         ioPins.push_back(num);
      }

      size_t index = ioPins[0] / 2; // the first number of iopins is the id
      CirGate* and_gate = new AndGate(ioPins, index, lineNo);
      // cout << and_gate->_fanin[0].get_gate() << " " << and_gate->_fanin[1].get_gate() << endl;
      _totalList[index] = and_gate;
      // myStr2Int(pins[0], tmp); cout << tmp << " ";
      // myStr2Int(pins[1], tmp); cout << tmp << " ";
      // myStr2Int(pins[2], tmp); cout << tmp << endl;
   }

   return true;
}

bool
CirMgr::read_Sym_Com(ifstream& f, size_t& lineNo)
{
   string line;
   while(getline(f, line)) // still have lines
   {
      ++lineNo;
      if(line == "c")
      {
         getline(f, line);
         ++lineNo;
         comments.push_back(pair<string, size_t>(line, lineNo));
         // cout << line << " line: " << lineNo << endl;
      }
      else if(line[0] == 'i' || line[0] == 'o')
      {
         symbols.push_back(pair<string, size_t>(line, lineNo));
         vector<string> syms;
         string sym;
         size_t n;

         n = myStrGetTok(line, sym);
         while (sym.size()) 
         {
            syms.push_back(sym);
            n = myStrGetTok(line, sym, n);
         }

         if(syms.size() == 1)
         {
            cerr << "[ERROR] Line " << lineNo << " Missing \"symbolic name\"!!" << endl;
            lineNo = 0; colNo = 0;
            return false;
         }

         if(syms[0][0] == 'i') //input
         {
            int num;
            myStr2Int(syms[0].substr(1), num);
            _totalList[num + 1]->_symbol = syms[1];
            // cout << num << " : " << _totalList[num + 1]->_symbol << endl;
         }
         else if(syms[0][0] == 'o')
         {
            int num;
            myStr2Int(syms[0].substr(1), num);
            _totalList[num + _M + 1]->_symbol = syms[1];
            // cout << num << " : " << _totalList[num + _M + 1]->_symbol << endl;
         }
      }
      // else
      // {
      //    cerr << "[ERROR] Line " << lineNo << ", Col 1: Illegal symbol type ( )!!" << endl;
      //    lineNo = 0; colNo = 0;
      //    return false;
      // }
   }

   return true;
}

bool
CirMgr::readCircuit(const string& fileName)
{
   string line;
   size_t lineNo;
   ifstream f(fileName);
   vector<string> nums;
   
   if(f.is_open())
   {
      // parse the header line
      // cout << "read header" << endl;
      if(!readHeader(f, nums)) return false;
      if(_M == 0 && _I == 0 && _L == 0 && _O == 0 && _A == 0) // zero circuit
      {
         return true;
      }
      // cout << M << " " << I << " " << L << " " << O << " " << A << endl;
      _capacity = _M + _O + 1;
      // _piList.reserve(_capacity); _poList.reserve(_capacity);
      _totalList.reserve(_capacity); _dfsList.reserve(_capacity);
      // insert const0 gate into the list
      CirGate* const0 = new constGate();
      // _piList[0] = const0;
      _totalList[0] = const0;
      // read other gates
      lineNo = 1;
      // read input gate
      // cout << "read PI" << endl;
      readPI(f, _I, lineNo);
      // read output gate
      // cout << "read PO" << endl;
      readPO(f, _O, lineNo); 
      // read And gate
      // cout << "read AIG" << endl;
      readAIG(f, _A, lineNo);
      // handle symbol and comment
      // cout << "read sym" << endl;      
      read_Sym_Com(f, lineNo);

      // connect all gates
      // cout << "connect" << endl;
      connect();
      // apply dfs to total list
      // cout << "genDFSList" << endl;
      genDFSlist();
      // check for floating gate
      // genFloatingList();
      
      f.close();
      return true;
   }
   else
   {
      cout << "Cannot open design \"" << fileName << "\"!!" << endl;
      return false;
   }

}

bool
CirMgr::connect()
{
   // hint : look at the _type member in CirGate
   // int size = _totalList.size();
   // cout << _capacity << endl;
   for(int i = 0; i < _capacity; ++i)
   {
      // if undefine gate
      // cout << i << endl;
      // cout << (size_t)_totalList[i] << endl;
      // if(size_t(_totalList[i]) == 0) continue
      if(size_t(_totalList[i]) == 0)
      {
         CirGate* un = new undefGate(i);
         _totalList[i] = un;
         continue;
      }
      if(_totalList[i]->type() == UNDEF_GATE)
      {
         // cout << "UNDEF" << endl;
         _totalList[i]->_num = i;
      }
      // cout << "some" << endl;
      // if input gate
      if(_totalList[i]->type() == PI_GATE) continue;
      // if output gate
      if(_totalList[i]->type() == PO_GATE)
      {
         // cout << "connect gate" << endl;
         connect_gate(i, PO_GATE);
      }
      // if And gate
      if(_totalList[i]->type() == AIG_GATE)
      {
         // cout << "connect gate" << endl;
         connect_gate(i, AIG_GATE);
      }

   }
}

bool
CirMgr::connect_gate(int& i, GateType gate_type)
{
   if(_totalList[i]->type() == UNDEF_GATE || _totalList[i]->type() == PI_GATE) return 0;
   else
   {
      if(gate_type == PO_GATE)
      {
         size_t num = _totalList[i]->_fanin[0].get_gate();
         if(_totalList[num]->type() == UNDEF_GATE)
         {
            _totalList[i]->_fanin[0].set_gate(0);
            // no input, floating gate
            _totalList[i]->_float = 1;
            return 0;
         }
         else 
         {
            _totalList[i]->_fanin[0].set_gate(size_t(_totalList[num]));
            _totalList[num]->_unused = 0;
            // just for printint fanout
            _totalList[num]->_fanout.push_back(CirGateType(_totalList[i], _totalList[i]->_fanin[0].isInv()));
         }
      }
      else // And gate
      {
         size_t in1 = _totalList[i]->_fanin[0].get_gate();
         size_t in2 = _totalList[i]->_fanin[1].get_gate();
         
         if(_totalList[in1]->type() == UNDEF_GATE)
         {
            _totalList[i]->_fanin[0].set_gate(0);
            // in1 is undefined, floating gate
            _totalList[i]->_float = 1;
            return 0;
         } 
         else
         {
            _totalList[i]->_fanin[0].set_gate(size_t(_totalList[in1]));
            _totalList[in1]->_unused = 0;
            _totalList[in1]->_fanout.push_back(CirGateType(_totalList[i], _totalList[i]->_fanin[0].isInv()));
         } 
         
         if(_totalList[in2]->type() == UNDEF_GATE) 
         {
            _totalList[i]->_fanin[1].set_gate(0);
            // in2 is undefined, floating gate
            _totalList[i]->_float = 1;
            return 0;
         }
         else
         {
            _totalList[i]->_fanin[1].set_gate(size_t(_totalList[in2]));
            _totalList[in2]->_unused = 0;      
            _totalList[in2]->_fanout.push_back(CirGateType(_totalList[i], _totalList[i]->_fanin[1].isInv()));
         } 
      }
   } 
   
   return true;
}

bool
CirMgr::genDFSlist()
{
   CirGate::_global_ref += 1;
   // size_t size = _poList.size();
   for(int i = 0; i < _capacity; ++i)
   {
      if(_totalList[i]->type() == PO_GATE)
      {
         dfs_search(_totalList[i]);
      }
   }
   return true;
}

void
CirMgr::dfs_search(CirGate* p)
{
   if(p->type() == UNDEF_GATE) // if p is undefined, then just return
   {
      p->_ref = p->_global_ref;
      return ;
   }
   else if(p->type() == PI_GATE || p->type() == CONST_GATE)
   {
      if(p->_ref != p->_global_ref) // checko if we meet this gate before
      {
         p->_ref = p->_global_ref;
         _dfsList.push_back(p);
      }
   }
   else if(p->type() == PO_GATE)
   {
      // CirGate* a = p->_fanin[0].gate();
      if(p->_ref != p->_global_ref)
      {
         p->_ref = p->_global_ref;
         dfs_search(p->_fanin[0].gate());
         _dfsList.push_back(p);
      }
      
   }
   else if(p->type() == AIG_GATE)
   {
      // CirGate* in1 = p->_fanin[0].gate();
      // CirGate* in2 = p->_fanin[1].gate();
      if(p->_ref != p->_global_ref)
      {
         p->_ref = p->_global_ref;
         dfs_search(p->_fanin[0].gate());
         dfs_search(p->_fanin[1].gate());
         _dfsList.push_back(p);
      }
      
   }
   
}

void
CirMgr::genFloatingList()
{
   for(int i = 0; i < _capacity; ++i)
   {
      if(_totalList[i]->_float == 1)
      {
         _floatList.insert(i);
      }
      
      if(_totalList[i]->_unused == 1)
      {
         _unusedList.insert(i);
      }
      
   }
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << setw(4) << "PI" << setw(12) << _I << endl;
   cout << setw(4) << "PO" << setw(12) << _O << endl;   
   cout << setw(5) << "AIG" << setw(11) << _A << endl;      
   cout << "------------------" << endl;
   cout << setw(7) << "Total" << setw(9) << _I + _O + _A << endl;   
}

void
CirMgr::printNetlist() const
{
   size_t size = _dfsList.size();
   for(int i = 0; i < size; ++i)
   {
      cout << "[" << i << "] ";
      _dfsList[i]->printGate(cout);
      cout << endl;
   }
}

void
CirMgr::AIG_Print_Fanin(size_t i, int index, ostream& os) const
{
   CirGateType p = _totalList[i]->_fanin[index];
   if(p.get_gate() != 0)
   {
      // not in the floating list
      if(p.isInv())
         os << " !" << p.gate()->id();
      else
         os << " " << p.gate()->id();
   }
   else
   {
      if(p.isInv())
         os << " *!" << p.gate()->id();
      else
         os << " *" << p.gate()->id();
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   // size_t size = _piList.size();
   for(int i = 0; i < _capacity; ++i)
   {
      if(_totalList[i]->type() == PI_GATE)
         cout << " " << _totalList[i]->id();
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   // size_t size = _poList.size();
   for(int i = 0; i < _capacity; ++i)
   {
      if(_totalList[i]->type() == PO_GATE)
         cout << " " <<_totalList[i]->id();
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   cout << "Gates with floating fanin(s):";
   set<size_t>::iterator iter;
   for(iter = _floatList.begin(); iter != _floatList.end(); ++iter)
   {
      cout << " " << *iter;
   }
   cout << endl;

   cout << "Gates defined but not used  :";
   for(iter = _unusedList.begin(); iter != _unusedList.end(); ++iter)
   {
      cout << " " << *iter;
   }
   cout << endl;
}

void
CirMgr::writeAag(ostream& outfile) const
{
   // size_t size = _totalList.size();
   outfile << "aag " << _M << " " << _I << " " << _L << " " << _O << " " << _A << endl;
   for(int i = 0; i < _I; ++i)
   {  
      outfile << _totalList[i + 1]->id() * 2 << endl;
   }

   for(int i = 0; i < _O; ++i)
   {
      outfile << _totalList[i + _M + 1]->_num * 2 + _totalList[i + _M + 1]->_fanin[0].isInv() << endl;
   }

   for(int i = 0; i < _dfsList.size(); ++i)
   {
      if(_dfsList[i]->type() == AIG_GATE)
      {
         outfile << _dfsList[i]->id() * 2;
         // cout << i << " " << _totalList[i]->_fanin[0].isInv() << " " << _totalList[i]->_fanin[1].isInv() << endl;
         writeGate(_dfsList[i]->_fanin[0].gate(), _dfsList[i]->_fanin[0].isInv(), outfile);
         writeGate(_dfsList[i]->_fanin[1].gate(), _dfsList[i]->_fanin[1].isInv(), outfile);
         outfile << endl;
      }
   }

   for(int i = 0; i < symbols.size(); ++i)
   {
      outfile << symbols[i].first << endl;
   }

   // for(int i = 0; i < comments.size(); ++i)
   // {
   //    outfile << "c" << endl << comments[i].first << endl;
   // }

}

void
CirMgr::writeGate(CirGate* p, bool inv, ostream& os) const
{
   if(p->type() == PI_GATE)
   {
      os << " " << p->id() * 2 + inv;
   }
   else if(p->type() == PO_GATE)
   {
      os << " " << p->num() * 2 + inv;
   }
   else if(p->type() == AIG_GATE)
   {
      os << " " << p->id() * 2 + inv;
   }
   
}

CirGate* 
CirMgr::getGate(unsigned gid) const
{
   return gid > _capacity ? 0 : _totalList[gid];
   // return _totalList[gid]->type() == UNDEF_GATE ? 0 : _totalList[gid];
}


