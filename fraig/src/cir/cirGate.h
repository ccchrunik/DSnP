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
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;
class CirGateType;
//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGateType
{
public:
   friend class CirMgr;
   #define NEG 0x1
   CirGateType(CirGate* a, size_t phase) { _gate = size_t(a) + phase; _phase = phase; }
   CirGateType(size_t a, size_t phase) { _gate = a + phase; _phase = phase; }
   CirGate* gate() const { return (CirGate*)(_gate & ~size_t(NEG)); }
   // size_t get_gate() const { return _gate & ~size_t(NEG); }
   size_t get_gate() const { return _gate - _phase; }
   void set_gate(size_t g) { _gate = g + _phase; }
   void set0() { _gate = 0; }
   bool isInv() const { return (_gate & NEG); }
   bool phase() const { return _phase; }

private:
   size_t _gate;
   bool _phase;
};

class CirGate
{
public:
   friend class CirMgr;
   CirGate(){ _type = UNDEF_GATE; _ref = 0; _symbol = ""; _float = 0; _unused = 1; _sim64 = 0;}
   virtual ~CirGate() {}
   static size_t _global_ref;   
   // Basic access methods
   string getTypeStr() const 
   {  
      string gateType;
      if(_type == CONST_GATE) gateType = "CONST0";
      else if(_type == PI_GATE) gateType = "PI";
      else if(_type == PO_GATE) gateType = "PO";
      else if(_type == AIG_GATE) gateType = "AIG";
      else gateType = "UNDEF";
      return gateType; 
   }
   void set_sim64(size_t v) { _sim64 = v; }
   size_t get_sim64() const { return _sim64; }
   size_t get_inv_sim64() const { return ~_sim64; }
   void op_sim_64()
   {
      if(_type == AIG_GATE)
      {
         CirGateType& c0 = _fanin[0];
         CirGateType& c1 = _fanin[1];
         size_t a0, a1;
         a0 = (c0.isInv()) ? ~c0.gate()->_sim64 : c0.gate()->_sim64;
         a1 = (c1.isInv()) ? ~c1.gate()->_sim64 : c1.gate()->_sim64;
         _sim64 =  (a0 & a1);
      }
      else if(_type == PO_GATE)
      {
         CirGateType& c = _fanin[0];
         size_t a;
         a = c.isInv() ? ~c.gate()->_sim64 : c.gate()->_sim64; 
         _sim64 = a;
      }
   }
   unsigned getLineNo() const { return _lineNo; }
   virtual bool isAig() const { return _type == AIG_GATE; }
   size_t id() const { return _id; } 
   size_t line() const { return _lineNo; }   
   size_t num() const { return _num; }
   GateType type() const { return _type; }
   size_t& ref() { return _ref; }
   bool floating() const { return _float; }
   bool unused() const { return _unused; }
   string symbol() { return _symbol; }
   vector<CirGateType>& fanin() { return _fanin; }
   vector<CirGateType>& fanout() { return _fanout; }
   void setVar(Var& v) { _var = v;}
   Var getVar() const { return _var; }

   // Printing functions
   virtual void printGate(ostream&) const {}
   void reportGate() const;
   void reportFanin(int level);
   void printFaninGate(int, int, bool, CirGate*);
   void reportFanout(int level);
   void printFanoutGate(int, int, bool, CirGate*);

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
   size_t _id, _lineNo, _num, _ref, _sim64;
   bool _float, _unused;
   string _symbol;
   GateType _type;
   vector<CirGateType> _fanin;
   vector<CirGateType> _fanout;
   Var _var;
};

class CirPiGate : public CirGate
{
public:
   friend class CirMgr;
   CirPiGate() {}
   CirPiGate(size_t num, size_t& lineNo) 
   { 
     _id = num / 2; _lineNo = lineNo; _type = PI_GATE; 
     _ref = 0; _symbol = "PI"; _float = 0; _unused = 1; _sim64 = 0;
   }
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
      _ref = 0; _symbol = "PO"; _float = 0; _unused = 1; _sim64 = 0;
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
            os << _id << " !" << _fanin[0].gate()->id(); // << " (" << _id << "GAT$PO)" << endl;
         else
            os << _id << " " << _fanin[0].gate()->id(); // << " (" << _id << "GAT$PO)" << endl;
      }
      else
      {
         if(_fanin[0].isInv()) 
            os << _id << " !" << _fanin[0].gate()->id() << " (" << _symbol << ")";
         else
            os << _id << " " << _fanin[0].gate()->id() << " (" << _symbol << ")";
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
      _ref = 0; _symbol = "AIG"; _float = 0; _unused = 1; _sim64 = 0;      
      _id = id; _lineNo = lineNo; _io = io; _type = AIG_GATE;
      po_inv = io[0] % 2 == 0 ? 0 : 1;
      pi_inv0 = io[1] % 2 == 0 ? 0 : 1; 
      pi_inv1 = io[2] % 2 == 0 ? 0 : 1; 
      size_t in0_gate = (io[1] - pi_inv0) / 2;
      size_t in1_gate = (io[2] - pi_inv1) / 2;
      _fanin.push_back(CirGateType((CirGate*)(in0_gate), pi_inv0));
      _fanin.push_back(CirGateType((CirGate*)(in1_gate), pi_inv1));
   }
   ~AndGate() { _fanin.clear(); _fanout.clear(); }
   size_t _num() { return _io[0]; }

   void printGate(ostream& os) const
   {
      os << "AIG ";
      os << _id;
      CirGateType in0 = _fanin[0];
      CirGateType in1 = _fanin[1];
      if(in0.get_gate() != 0) // not in the floating list, that is has input
      {
         if(in0.isInv()) os << " !" << in0.gate()->id();
         else os << " " << in0.gate()->id();
      }
      else
      {
         if(in0.isInv()) os << " *!" << in0.gate()->id();
         else os << " *" << in0.gate()->id();
      }

      if(in1.get_gate() != 0) // not in the floating list, that is has input
      {
         if(in1.isInv()) os << " !" << in1.gate()->id();
         else os << " " << in1.gate()->id();
      }
      else
      {
         if(in1.isInv()) os << " *!" << in1.gate()->id();
         else os << " *" << in1.gate()->id();
      }
   }
private:  
   vector<size_t> _io;
   bool po_inv, pi_inv0, pi_inv1;
};

class undefGate : public CirGate
{
public:
   undefGate() { _type = UNDEF_GATE; _id = 0; _ref = 0; _symbol = "UNDEF"; _float = 1; _unused = 1; _sim64 = 0; }
   undefGate(size_t num) { _type = UNDEF_GATE; _id = 0; _num = num; }
   ~undefGate() { _fanin.clear(); _fanout.clear(); }
   void printGate(ostream& os) const { os << "UNDEF"; }

};

class constGate : public CirGate
{
public:
   constGate() 
   { 
      _type = CONST_GATE; _id = 0; _num = 0; _lineNo = 0; _ref = 0; 
      _symbol = "CONST0"; _float = 0; _unused = 1; _sim64 = 0; 
   }
   ~constGate() { _fanin.clear(); _fanout.clear(); }
   void printGate(ostream& os) const { os << "CONST0"; }
private:
};

#endif // CIR_GATE_H
