/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGateType
{
public:
   friend class CirMgr;
   #define NEG 0x1
   CirGateType(CirGate* a, size_t phase) { _gate = size_t(a) + phase; _phase = phase; }
   CirGateType(size_t a, size_t phase) { _gate = a + phase; _phase = phase; }
   CirGate* gate() const { return (CirGate*)(_gate & ~size_t(NEG)); }
   size_t get_gate() const { return _gate - phase(); }
   void set_gate(size_t g) { _gate = g + _phase; }
   bool isInv() const { return (_gate & NEG); }
   bool phase() const { return _phase; }

private:
   size_t _gate;
   bool _phase;
};

class CirGate
{
   static size_t _global_ref;
public:
   friend class CirMgr;
   
   CirGate() { _type = UNDEF_GATE; _ref = 0; _symbol = ""; _float = 0; _unused = 1; }
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { return ""; }
   unsigned getLineNo() const { return 0; }

   // Printing functions
   virtual void printGate(ostream&) const = 0;
   
   void reportGate() const;
   void reportFanin(int level);
   void printFaninGate(int, int, bool, CirGate*);
   void reportFanout(int level);
   void printFanoutGate(int, int, bool, CirGate*);
   size_t id() const { return _id; } 
   size_t line() const { return _lineNo; }   
   size_t num() const { return _num; }
   GateType type() const { return _type; }
   size_t checknum(size_t n) const
   {
      size_t num = n;
      size_t count = 0;
      while(num / 10 > 0)
      {
         num /= 10;
         ++count;
      }
      return count;
   }

private:

protected:
   size_t _id, _lineNo, _num, _ref;
   bool _float, _unused;
   string _symbol;
   GateType _type;
   vector<CirGateType> _fanin;
   vector<CirGateType> _fanout;

};

class CirPiGate : public CirGate
{
public:
   friend class CirMgr;
   CirPiGate() {}
   CirPiGate(size_t num, size_t& lineNo) { _id = num / 2; _lineNo = lineNo; _type = PI_GATE; }
   ~CirPiGate() { _fanin.clear(); _fanout.clear(); }
   size_t num() { return _id * 2; }
   void printGate(ostream& os) const
   {
      if(_symbol == "")
         os << "PI  " << _id; // << " (" << _id << "GAT)" << endl;
      else
         os << "PI  " << _id << " (" << _symbol << ")";         
   }
private:
};

class CirPoGate : public CirGate
{
public:
   friend class CirMgr;
   CirPoGate() {}
   CirPoGate(size_t num, size_t& lineNo, size_t& id) 
   { 
      _lineNo = lineNo; _id = id; _type = PO_GATE;
      _inv = (num % 2 == 0) ? 0 : 1; 
      _num = (num - _inv) / 2;
      _fanin.push_back(CirGateType((CirGate*)_num, _inv));
   }
   ~CirPoGate() { _fanin.clear(); _fanout.clear(); }
   void printGate(ostream& os) const
   {
      os << "PO  ";
      if(_symbol == "")
      {
         if(_fanin[0].isInv()) 
            os << _id << " !" << _num; // << " (" << _id << "GAT$PO)" << endl;
         else
            os << _id << " " << _num; // << " (" << _id << "GAT$PO)" << endl;
      }
      else
      {
         if(_fanin[0].isInv()) 
            os << _id << " !" << _num << " (" << _symbol << ")";
         else
            os << _id << " " << _num << " (" << _symbol << ")";
      }
   }
private:  
   // _num is the raw fanin
   bool _inv;
};

class AndGate : public CirGate
{
public:
   friend class CirMgr;
   AndGate() {}
   AndGate(vector<size_t>& io, size_t& id, size_t& lineNo) 
   { 
      _id = id; _lineNo = lineNo; _io = io; _type = AIG_GATE;
      po_inv = io[0] % 2 == 0 ? 0 : 1;
      pi_inv0 = io[1] % 2 == 0 ? 0 : 1; 
      pi_inv1 = io[2] % 2 == 0 ? 0 : 1; 
      // size_t out_gate = (io[0] - po_inv) / 2;
      size_t in1_gate = (io[1] - pi_inv0) / 2;
      size_t in2_gate = (io[2] - pi_inv1) / 2;
      // cout << out_gate << " " << in1_gate << " " << in2_gate << endl;
      // cout << po_inv << " " << pi_inv0 << " " << pi_inv1 << endl;
      // _fanout.push_back(CirGateType((CirGate*)(out_gate), po_inv));
      _fanin.push_back(CirGateType((CirGate*)(in1_gate), pi_inv0));
      _fanin.push_back(CirGateType((CirGate*)(in2_gate), pi_inv1));
   }
   ~AndGate() { _fanin.clear(); _fanout.clear(); }
   size_t _num() { return _io[0]; }

   void printGate(ostream& os) const
   {
      os << "AIG ";
      os << _id;
      CirGateType in1 = _fanin[0];
      CirGateType in2 = _fanin[1];
      if(in1.get_gate() != 0)// not in the floating list
      {
         if(in1.isInv()) os << " !" << in1.gate()->id();
         else os << " " << in1.gate()->id();
      }
      else
      {
         if(in1.isInv()) os << " *!" << in1.gate()->id();
         else os << " *" << in1.gate()->id();
      }

      if(in2.get_gate() != 0)// not in the floating list
      {
         if(in2.isInv()) os << " !" << in2.gate()->id();
         else os << " " << in2.gate()->id();
      }
      else
      {
         if(in2.isInv()) os << " *!" << in2.gate()->id();
         else os << " *" << in2.gate()->id();
      }
   }
private:  
   vector<size_t> _io;
   bool po_inv, pi_inv0, pi_inv1;
};

class undefGate : public CirGate
{
public:
   undefGate() { _type = UNDEF_GATE; _id = 0; }
   undefGate(size_t num) { _type = UNDEF_GATE; _id = 0; _num = num; }
   ~undefGate() { _fanin.clear(); }
   void printGate(ostream& os) const { os << "UNDEF" << endl; }

};

class constGate : public CirGate
{
public:
   constGate() { _type = CONST_GATE; _id = 0; _num = 0; _lineNo = 0; }
   ~constGate() { _fanin.clear(); }

   void printGate(ostream& os) const { os << "CONST0" << endl; }
private:
};




#endif // CIR_GATE_H
