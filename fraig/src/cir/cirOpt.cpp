/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
   // remove the unreachable gate, both floating and unused
   // do not remove any PIs

   // Mostly Done, but have some questions
   // Question: does undef gate should be removed ? 
   cout << "Now sweeping..." << endl;
   set<size_t> not_used;
   set<size_t>::iterator iter;
   
   // for all gate in unusedList, do sweepGate
   for(iter = _unusedList.begin(); iter != _unusedList.end(); ++iter)
   {
      // determine whether to remove the gate is written in sweepGate function
      sweepGate(_totalList[*iter], not_used);
   }
   
   // delete the not_used gate
   for(iter = not_used.begin(); iter != not_used.end(); ++iter)
   {
      GateType tp = _totalList[*iter]->type();
      if(tp == UNDEF_GATE)
      {
         cout << "sweeping: " << _totalList[*iter]->getTypeStr() << "(" << *iter << ") " << "removed ..." << endl;
         // delete _totalList[*iter];
      }
      if(tp != AIG_GATE) continue;
      // cout << "replacing unused gate to undef gate" << endl;
      cout << "sweeping: " << _totalList[*iter]->getTypeStr() << "(" << *iter << ") " << "removed ..." << endl;
      _A -= 1;
      // replace that number in the vector with an undef gate
      CirGate* r = _totalList[*iter];
      // if()
      // delete r; // r become a pointer point to 0x0
      if(r->type() != UNDEF_GATE)
      {
         _totalList[*iter] = new undefGate();
         // delete r;
      }
   }
   
   // generate the new flaoting list
   reCount();
   genFloatingList();
   
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
   // ****** Not yet to do
   // 1. link the replaced gate's input's output to the original output
   // Hint: can pass the CirGate* p and p's fanout to replace_fanin_opt, 
   //       then delete p in fanin(a)'s fanout list, and add all p's fanout
   //       into a's fanout list
   // 2. check the correctness of replace_fanout gate
   // 3. restructure this function  

   // scan through the dfsList for pruning out trivial gates
   for(int i = 0; i < _dfsList.size(); ++i)
   {
      CirGate* p = _dfsList[i];
      if(p->type() == AIG_GATE)
      {
         CirGateType in0 = p->_fanin[0];
         CirGateType in1 = p->_fanin[1];
         bool opt = 0;
         // CirGate* rem, rep;
         // rep = *in0.gate();
         if(in0.gate() == in1.gate() && (in0.isInv() == in1.isInv())) // that is, the output only depends on input, but not aig function
         {
            cout << "Simplifing: merging the same gate: " << in0.gate()->id() << endl;
            // first replace p's fanout's fanin to in0.gate()
            // then update p's fanin's p's place to in0.gate()
            replace_fanout_opt(p, in0.gate(), in0.isInv());
            replace_fanin_opt(p, in0.gate(), in0.isInv()); 
            opt = 1;
         }
         else if(in0.gate() == in1.gate() && (in0.isInv() != in1.isInv())) // opposite sign, definite become 0
         {
            cout << "Simplifing: 0 merging to the " << p->id() << endl;
            replace_fanout_opt(p, _totalList[0], false);
            replace_fanin_opt(p, _totalList[0], false);
            // since it can be considered to connect to CONST0, so we have to remove connection on in0 and in1
            clear_fanout_gate(in0.gate(), p); // clear p in in0's fanout
            clear_fanout_gate(in1.gate(), p); // clear p in in1's fanout
            opt = 1;
         }
         else if(in0.gate() == _totalList[0] && !in0.isInv()) // in0 is CONST0, then aig becomes 0
         {
            cout << "Simplifing: 0 merging to the " << p->id() << endl;
            replace_fanout_opt(p, _totalList[0], false);
            replace_fanin_opt(p, _totalList[0], false);
            clear_fanout_gate(in1.gate(), p); // clear p in in1's fanout
            opt = 1;
         }
         else if(in1.gate() == _totalList[0] && !in1.isInv()) // in1 is CONST0, then aig becomes 0
         {
            cout << "Simplifing: 0 merging to the " << p->id() << endl;
            replace_fanout_opt(p, _totalList[0], false);
            replace_fanin_opt(p, _totalList[0], false);
            clear_fanout_gate(in0.gate(), p); // clear p in in0's fanout
            opt = 1;
         }
         else if(in0.gate() == _totalList[0] && in0.isInv()) // in0 is CONST1, then aig only depends on in1
         {
            string s = in1.isInv() ? "!" : "";
            cout << "Simplifing: " << s << in1.gate()->id() << " merging to " << p->id() << endl;
            replace_fanout_opt(p, in1.gate(), in1.isInv());
            replace_fanin_opt(p, in1.gate(), in1.isInv());
            clear_fanout_gate(_totalList[0], p); // clear p in CONST0's fanout
            opt = 1;
         }
         else if(in1.gate() == _totalList[0] && in1.isInv()) // in1 is CONST1, then aig only depends on in0
         {
            string s = in0.isInv() ? "!" : "";
            cout << "Simplifing: " << in0.gate()->id() << " merging to " << p->id() << endl;
            replace_fanout_opt(p, in0.gate(), in0.isInv());
            replace_fanin_opt(p, in0.gate(), in0.isInv());
            clear_fanout_gate(_totalList[0], p); // clear p in CONST0's fanout
            opt = 1;
         }

         if(opt) // if we 'do' prune out some gates
         {
            // delete p
            CirGate* r = _dfsList[i];
            int idx = r->id();
            // _totalList[i] = new undefGate(r->line());
            _totalList[idx] = new undefGate();
            delete r;   
            // _totalList[i] = 0;              
         }
      }

   }

   // update dfsList 
   reCount();
   genDFSlist();
   genFloatingList();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/

// ---------sweep-----------

void
CirMgr::sweepGate(CirGate* p, set<size_t>& r)
{
   if(p == 0) return;
   if(p->type() == UNDEF_GATE)
   {
      r.insert(p->id());
      return ;
   }
   // p is the to-be-removed gate
   // first delete the p's all fanin's output link to p
   // not sure undef, const, pi gate may lead to some error
   // cout << "sweeping gate: " << p->id() << endl;
   vector<CirGateType>& fi = p->_fanin;

   // 1. for each remove gate, first find all its fanin's fanout
   // 2. then remove all p in that fanout
   // Hint: may ecounter undef gate, so if met, then just add it to remove list and skip
   // 3. to check if some fanin become unused
   for(int i = 0; i < fi.size(); ++i)
   {
      // cout << "  clear fanout's gate" << endl;
      if(fi[i].gate() == 0) continue; // undef gate 
      vector<CirGateType>& fo = fi[i].gate()->_fanout;
      // ideally, any element in _fanout should not be undefined
      vector<size_t> rem;
      for(int j = 0; j < fo.size(); ++j)
      {
         if(fo[j].gate() == p)
         {
            // fo.erase(fo.begin() + j);
            rem.push_back(j);
         }
      }

      for(int k = rem.size() - 1; k >= 0; --k)
      {
         // remove from back to head to prevent index error
         fo.erase(fo.begin() + rem[k]);
      }
   }

   // then check that if some fanin's fanout are empty, then call sweepGate again
   for(int i = 0; i < fi.size(); ++i)
   {
      // cout << "  checking if empty" << endl;
      if(fi[i].gate() == 0) continue; // undef gate
      vector<CirGateType>& fo = fi[i].gate()->_fanout;
      if(fo.empty()) // no fanout, means unused, then call sweepGate
      {
         sweepGate(fi[i].gate(), r);
      }
   }

   // last, decide whether to remove the gate
   if(p->type() == UNDEF_GATE || p->type() == AIG_GATE) r.insert(p->id());     
   //  if(p->type() == PO_GATE || p->type() == AIG_GATE) r.insert(p->id());
}

// -------optimize----------

void
CirMgr::clear_fanin_gate(CirGate* p, CirGate* g)
{
   vector<CirGateType>& fi = p->_fanin;
   vector<size_t> rem;
   for(int i = 0; i < fi.size(); ++i)
   {
      if(fi[i].gate() == g)
      {
         // fi.erase(fi.begin() + i);
         rem.push_back(i);
      }
   }

   for(int k = rem.size() - 1; k >= 0; --k)
   {
      fi.erase(fi.begin() + rem[k]);
   }
}

void
CirMgr::clear_fanout_gate(CirGate* p, CirGate* g)
{
   vector<CirGateType>& fo = p->_fanout;
   vector<size_t> rem;
   for(int i = 0; i < fo.size(); ++i)
   {
      if(fo[i].gate() == g)
      {
         // fo.erase(fo.begin() + i);
         rem.push_back(i);
      }
   }

   for(int k = rem.size() - 1; k >= 0; --k)
   {
      fo.erase(fo.begin() + rem[k]);
   }
}

void
CirMgr::replace_fanout_opt(CirGate* p, CirGate* g, bool inv)
{
   // replace p's fanout's "fanin" to g
   // p just providing its fanout, to link all fanout to g
   // p is the to-be-replaced gate, g is the replacing one, that is, p -> g
   vector<CirGateType>& fo = p->_fanout;
   for(int i = 0; i < fo.size(); ++i)
   {
      // to replace the original gate in the output's gate's fanin list
      vector<CirGateType>& fi = fo[i].gate()->_fanin;
      for(int j = 0; j < fi.size(); ++j)
      {
         if(fi[j].gate() == p)
         {
            CirGateType r(g, inv ^ fo[i].isInv()); // 1 xor 1 = 0 , 0 xor 1 = 1
            fi[j] = r; // replace it with r
         }
      }
   }
}

void 
CirMgr::replace_fanin_opt(CirGate* p, CirGate* g, bool inv) 
{
   // p is the to-be-replaced gate, g is the to-replace-to gate
   // p just provides the fanin, link all fanin's "fanout" to g
   // Since only have in0 and in1, so g is a specific gate
   vector<CirGateType>& go = g->_fanout;
   vector<size_t> rem;
   for(int i = 0; i < go.size(); ++i)
   {
      if(go[i].gate() == p)
      {
         rem.push_back(i);
      }
   }

   for(int k = rem.size() - 1; k >= 0; --k)
   {
      go.erase(go.begin() + rem[k]); // delete from back to prevent indexing error
   }

   vector<CirGateType>& fo = p->_fanout;
   for(int i = 0; i < fo.size(); ++i)
   {
      CirGateType r(fo[i].gate(), inv ^ fo[i].isInv());
      go.push_back(r);
   }

}




