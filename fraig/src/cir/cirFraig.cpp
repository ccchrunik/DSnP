/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"
#include <unordered_map>

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

class HashKey
{
public:
   HashKey(CirGate* p) 
   { 
      _type = p->type();
      CirGateType i_0 = p->fanin()[0]; CirGateType i_1 = p->fanin()[1];
      in0 = i_0.gate()->id(); in1 = i_1.gate()->id();
      inv_0 = i_0.isInv(); inv_1 = i_1.isInv();
      hash_v = 0; hash_func();
   }
   size_t operator() () const { return hash_v; }
   bool operator== ( const HashKey& k) const { return hash_value() == k.hash_value(); }
   size_t hash_value() const { return hash_v; }
   GateType type() const { return _type; }
   size_t in_0() const { return in0; }
   size_t in_1() const { return in1; }
   bool inv0() const { return inv_0; }
   bool inv1() const { return inv_1; }

   void hash_func() 
   {  
      if(_type == AIG_GATE)
      {
         if(in0 >= in1)
         {
            hash_v = (in0 << 40) + (in1 << 30) + ((size_t)inv_0 << 20) + ((size_t)inv_1 << 10);
         }
         else
         {
            hash_v = (in1 << 40) + (in0 << 30) + ((size_t)inv_1 << 20) + ((size_t)inv_0 << 10);
         }
      }
   }

private:
   size_t in0, in1;
   bool inv_0, inv_1;
   GateType _type;
   size_t hash_v;
};

/*******************************/
/*   Global variable and enum  */
/*******************************/
typedef unordered_map<HashKey, CirGate*> STHASH;
typedef pair<HashKey, CirGate*> STPair;
/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed



namespace std {

  template <>
  struct hash<HashKey>
  {
    std::size_t operator()(const HashKey& k) const
    {
      using std::size_t;
      using std::hash;

      return k.hash_value();
    }
  };

}

void
CirMgr::strash()
{
  // not yet to implement hash function
  // 1. Follow the order of DFSlist to push each gate into the hash
  // 2. Consider using the the combination of the id(and maybe lineNo or num) of all input to calculate the hash
  //    , where hash key is the value after hash function and hash value is the gate that hash such fanin
  // 3. If collision happens, we can merge the following gate to the gate in the hash before
  // 4. How to merge : just call replace_fanout_gate() and replace_fanin_gate() to replace the gate to that gate in the hash 
  // 5. At last, we may try to refresh netlist and do reconnect, getDFSlist and so on again 

   STHASH st_hash;
   for(int i = 0; i < _dfsList.size(); ++i)
   {
      if(_dfsList[i] == 0) continue;
      if(_dfsList[i]->type() != AIG_GATE) continue;
      HashKey h(_dfsList[i]);
      STHASH::iterator it;
      it = st_hash.find(h);
      if(it != st_hash.end() && sth_check(h, it->first)) // already has an element, merge
      {
         cout << "Strashing: " << it->second->id() << " merging to " << _dfsList[i]->id() << endl;
         st_merge(_dfsList[i], it->second); // (to merge, keep)
         // _A -= 1;
         CirGate* r = _dfsList[i];
         _dfsList[i] = new undefGate();
         delete r;
      }
      else // no collision, insert it into hash
      {
         st_hash.insert(STPair(h, _dfsList[i]));
      }
   }
   
   // reconnect all gate
   // connect();
   reCount();
   genDFSlist();
   genFloatingList();
}

void
CirMgr::fraig()
{
   SatSolver solver;
   solver.initialize();

   genProofModel(solver);
   // cout << "totalList size: " << _totalList.size() << endl;
   // cout << _totalList[1]->id() << endl;
   for(int i = 0; i < FEC_GROUPS.size(); ++i)
   {
      fraig_solver(solver, FEC_GROUPS[i]);
   }

   // Since we simplify the circuit before, so we have to generate a new dfsList
   // cout << "calling reCount()" << endl;
   reCount();
   // cout << "calling genDFSList()" << endl;
   genDFSlist();
   // cout << "calling genFloatingList()" << endl;
   genFloatingList();
}

/********************************************/
/*   Private member functions about strash  */
/********************************************/
void
CirMgr::st_merge(CirGate* p, CirGate* g)
{
   // g keeps and p merged
   replace_fanout_sth(p, g);
   replace_fanin_sth(p, g);
}

bool 
CirMgr::sth_check(HashKey a, HashKey b)
{
   if(a.type() != b.type()) return false;
   size_t a0, a1, ai0, ai1;
   size_t b0, b1, bi0, bi1;

   if(a.in_0() >= a.in_1()) // 5, 2
   {
      if(b.in_0() >= b.in_1()) // 4, 3
      {
         a0 = a.in_0(); a1 = a.in_1(); ai0 = a.inv0(); ai1 = a.inv1();
         b0 = b.in_0(); b1 = b.in_1(); bi0 = b.inv0(); bi1 = b.inv1(); 
      }
      else // 3, 4
      {
         a0 = a.in_0(); a1 = a.in_1(); ai0 = a.inv0(); ai1 = a.inv1();
         b0 = b.in_1(); b1 = b.in_0(); bi0 = b.inv1(); bi1 = b.inv0(); 
      }
   } 
   else // 2, 5
   {
      if(b.in_0() >= b.in_1()) // 4, 3
      {
         a0 = a.in_1(); a1 = a.in_0(); ai0 = a.inv1(); ai1 = a.inv0();
         b0 = b.in_0(); b1 = b.in_1(); bi0 = b.inv0(); bi1 = b.inv1(); 
      }
      else // 3, 4
      {
         a0 = a.in_1(); a1 = a.in_0(); ai0 = a.inv1(); ai1 = a.inv0();
         b0 = b.in_1(); b1 = b.in_0(); bi0 = b.inv1(); bi1 = b.inv0(); 
      }

   }

   if(a0 == b0 && a1 == b1 && ai0 == bi0 && ai1 == bi1)
   {
      return true;
   }
   else
   {
      return false;
   }

}

void
CirMgr::replace_fanout_sth(CirGate* p, CirGate* g)
{
   // replace p to g
   vector<CirGateType>& fo = p->_fanout;
   for(int i = 0; i < fo.size(); ++i)
   {
      // to replace the original gate in the output's gate's fanin list
      vector<CirGateType>& fi = fo[i].gate()->_fanin;
      for(int j = 0; j < fi.size(); ++j)
      {
         if(fi[j].gate() == p)
         {
            CirGateType r(g, fi[j].isInv());   
            fi[j] = r; // replace it with r
         }
      }
   }
}


void 
CirMgr::replace_fanin_sth(CirGate* p, CirGate* g) 
{
   // p is the given gate, g is the gate that p is to be replaced to
   vector<CirGateType>& fi = p->_fanin;
   // replace g for p in all p's fanin
   for(int i = 0; i < fi.size(); ++i)
   {
      // for each p's fanin
      vector<CirGateType>& fo = fi[i].gate()->_fanout;
      for(int j = 0; j < fo.size(); ++j)
      {
         // find which element is p
         if(fo[j].gate() == p)
         {
            // replace p with g
            CirGateType c(g, fo[j].isInv());
            fo[j] = c;
         }
      }
      
   }

}



/********************************************/
/*   Private member functions about fraig   */
/********************************************/

void
CirMgr::replace_fanout_frg(GateV& a, GateV& b)
{
   // g merge to p
   CirGate* p ; CirGate* g;
   p = a.first; g = b.first; 
   // 0 ^ 1 or 1 ^ 0, a = !b; 0 ^ 0 or 1 ^ 1, a == b 
   bool inv = a.second ^ b.second;
   vector<CirGateType>& fo = g->fanout();
   // replace g's fanout's fanin to p
   for(int i = 0; i < fo.size(); ++i)
   {
      vector<CirGateType>& fi = fo[i].gate()->fanin();
      for(int j = 0; j < fi.size(); ++j)
      {
         if(fi[j].gate() == g)
         {
            CirGateType r(p, fi[j].isInv() ^ inv); // since a = !b
            fi[j] = r;
         }
      }
   }

}

void
CirMgr::replace_fanin_frg(GateV& a, GateV& b)
{
   // g merge to p
   CirGate* p ; CirGate* g;
   p = a.first; g = b.first; 
   // 0 ^ 1 or 1 ^ 0, a = !b; 0 ^ 0 or 1 ^ 1, a == b 
   bool inv = a.second ^ b.second;
   vector<CirGateType>& fi = g->fanin();
   // replace g's fanout's fanin to p
   for(int i = 0; i < fi.size(); ++i)
   {
      vector<CirGateType>& fo = fi[i].gate()->fanout();
      for(int j = 0; j < fo.size(); ++j)
      {
         if(fo[j].gate() == g)
         {
            CirGateType r(p, fo[j].isInv() ^ inv); // since a = !b
            fo[j] = r;
         }
      }
   }
   
}

void
CirMgr::genProofModel(SatSolver& s)
{
   for(int i = 0; i < _dfsList.size(); ++i)
   {
      Var v = s.newVar();
      _dfsList[i]->setVar(v);
   }

   for(int i = 0; i < _dfsList.size(); ++i)
   {
      if(_dfsList[i]->type() == AIG_GATE)
      {
         vector<CirGateType>& fi = _dfsList[i]->_fanin;
         s.addAigCNF(_dfsList[i]->getVar(), fi[0].gate()->getVar(), fi[0].isInv(),
                     fi[1].gate()->getVar(), fi[1].isInv());
      }
   }
}

void
CirMgr::reportResult(const SatSolver& solver, bool result)
{
   solver.printStats();
   cout << (result? "SAT" : "UNSAT") << endl;
   if (result) {
      for (size_t i = 0, n = _dfsList.size(); i < n; ++i)
         cout << solver.getValue(_dfsList[i]->getVar()) << endl;
   }
}

void 
CirMgr::fraig_solver(SatSolver& s, vector<GateV>& grp)
{
   assert(grp.size() > 0);
   vector<GateV> Tmp;
   // initial case
   Tmp.push_back(grp[0]);
   // for each gate in the grp, find pair to solve it
   // since grp.size() must >= 2
   for(int i = 1; i < grp.size(); ++i)
   {
      for(int j = 0; j < Tmp.size(); ++j)
      {
         bool result;
         Var newV = s.newVar();
         // !!!!!!!!!!!!!! not sure, examine the below code again
         s.addXorCNF(newV, grp[i].first->getVar(), grp[i].second, Tmp[j].first->getVar(), Tmp[j].second);
         s.assumeRelease();  // Clear assumptions
         s.assumeProperty(newV, true);  // k = 1
         result = s.assumpSolve();
         // reportResult(s, result);

         if(result == 0) // the same gate
         {
            // merging two gate
            // note that we should aware of the inverse sign
            string s = Tmp[j].second ^ grp[i].second ? "!" : "";
            cout << "merging: " << Tmp[j].first->id() << " to " << s << grp[i].first->id() << endl;
            replace_fanout_frg(Tmp[j], grp[i]);
            replace_fanin_frg(Tmp[j], grp[i]);
            grp[i].first->_fanin.clear();
            grp[i].first->_fanout.clear();
            // delete the duplicate gate
            size_t a = Tmp[j].first->id();
            // delete Tmp[j].first;
            _totalList[a] = new undefGate();
            break; // since we had been merge to the other gate
            
         }
      }
   }
}