/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
   if(type() == UNDEF_GATE)
   {
      size_t w1 = checknum(_num);
      cout << "==================================================" << endl;
      cout << "= " << "UNDEF(" << _num << ")" << setw(41 - w1) << "=" << endl; 
      cout << "==================================================" << endl;
   }
   else if(type() == CONST_GATE)
   {
      cout << "==================================================" << endl;
      cout << "= " << "CONST0" << setw(42) << "=" << endl;
      cout << "==================================================" << endl;
   }
   else if(type() == PI_GATE)
   {
      size_t w1 = checknum(id());
      size_t w2 = checknum(line());
      size_t w3 = _symbol.length();
      if(_symbol == "")
      {
         cout << "==================================================" << endl;
         cout << "= PI(" << id() << ")" << ", line " << line() << setw(35 - w1 - w2 - w3) << "=" << endl;
         cout << "==================================================" << endl;
      }
      else
      {
         cout << "==================================================" << endl;
         cout << "= PI(" << id() << ")" << "\"" << _symbol << "\"" << ", line " << line() << setw(35 - w1 - w2 - w3 - 2) << "=" << endl;
         cout << "==================================================" << endl;
      }
   }
   else if(type() == PO_GATE)
   {
      size_t w1 = checknum(id());
      size_t w2 = checknum(line());
      size_t w3 = _symbol.length();
      if(_symbol == "")
      {
         cout << "==================================================" << endl;
         cout << "= PO(" << id() << ")" << ", line " << line() << setw(35 - w1 - w2 - w3) << "=" << endl;
         cout << "==================================================" << endl;
      }
      else
      {
         cout << "==================================================" << endl;
         cout << "= PO(" << id() << ")" << "\"" << _symbol << "\"" << ", line " << line() << setw(35 - w1 - w2 - w3 - 2) << "=" << endl;
         cout << "==================================================" << endl;
      }
   }
   else if(type() == AIG_GATE)
   {
      size_t w1 = checknum(id());
      size_t w2 = checknum(line());
      cout << "==================================================" << endl;
      cout << "= AIG(" << id() << ")" << ", line " << line() << setw(34 - w1 - w2) << "=" << endl;
      cout << "==================================================" << endl;
   }
}

void
CirGate::reportFanin(int level) 
{
   assert (level >= 0);
   CirGate::_global_ref += 1;
   printFaninGate(0, level, 0, this);
}

void
CirGate::reportFanout(int level)
{
   assert (level >= 0);
   CirGate::_global_ref += 1;
   printFanoutGate(0, level, 0, this);
}

void 
CirGate::printFaninGate(int count, int level, bool inv, CirGate* p) 
{
   int _count = count * 2;

   if(p->type() == UNDEF_GATE)
   {
      p->_ref = p->_global_ref;
      string s(_count, ' ');
      cout << s << "UNDEF " << p->_num << endl;
      return ;
   }
   else if(p->type() == CONST_GATE)
   {
      p->_ref = p->_global_ref;
      string _inv = inv ? "!" : "";
      string s(_count, ' ');
      cout << s << _inv << "CONST0 " << 0 << endl; 
   }
   else if(p->type() == PI_GATE)
   {
      p->_ref = p->_global_ref;
      string _inv = inv ? "!" : "";
      string s(_count, ' ');
      cout << s << _inv << "PI " << p->id() << endl;
   }
   else if(p->type() == PO_GATE)
   {
      CirGateType& c = p->_fanin[0];
      string _inv = inv ? "!" : "";
      string s(_count, ' ');
      if(p->_ref != p->_global_ref)
      {
         // if we reach the fanin limit and still have
         cout << s << _inv << "PO " << p->id() << endl;
         if(count + 1 <= level && c.get_gate() != 0)
         {
            p->_ref = p->_global_ref;
            printFaninGate(count + 1, level, c.isInv(), c.gate());
         }
      }
      else // had been searched, so if count >= level, we should print (*) at the end of the line
      {
         // if we reach the fanin limit and we should print (*), else we print nothing
         string _end = (count >= level && c.get_gate() != 0) ? "" : " (*)" ; 
         cout << s << _inv << "PO " << p->id() << _end << endl;
      }
   }
   else if(p->type() == AIG_GATE)
   {
      CirGateType& c1 = p->_fanin[0];
      CirGateType& c2 = p->_fanin[1];
      string _inv = inv ? "!" : "";
      string s(_count, ' ');
      if(p->_ref != p->_global_ref)
      {
         cout << s << _inv << "AIG " << p->id() << endl;
         if(count + 1 <= level)
         {
            p->_ref = p->_global_ref;
            if(c1.get_gate() != 0)
            {
               printFaninGate(count + 1, level, c1.isInv(), c1.gate());
            }
            if(c2.get_gate() != 0)
            {
               printFaninGate(count + 1, level, c2.isInv(), c2.gate());
            }
         }
      }
      else
      {
         string _end = (count >= level && (c1.get_gate() != 0 || c2.get_gate() != 0)) ? "" : " (*)" ; 
         cout << s << _inv << "AIG " << p->id() << _end << endl;
      }
   }
}

void 
CirGate::printFanoutGate(int count, int level, bool inv, CirGate* p)
{
   if(size_t(p) == 0) return ;
   int _count = count * 2;
   string s_type;
   if(p->type() == UNDEF_GATE) s_type = "UNDEF ";
   else if(p->type() == CONST_GATE) s_type = "CONST0 ";
   else if(p->type() == PI_GATE) s_type = "PI ";
   else if(p->type() == PO_GATE) s_type = "PO ";
   else if(p->type() == AIG_GATE) s_type = "AIG ";

   // if(p->type() == UNDEF_GATE)
   // {
   //    p->_ref = p->_global_ref;
   //    string s(_count, ' ');
   //    cout << s << s_type << p->_num << endl;
   // }
   // else
   {
      string _inv = inv ? "!" : "";
      string s(_count, ' ');
      if(p->_ref != p->_global_ref) // not been traversed
      {
         cout << s << _inv << s_type << p->id() << endl;
         // if(count + 1 <= level)
         if(count + 1 <= level) // check the recursion limit
         {
            p->_ref = p->_global_ref;
            for(int i = 0; i < p->_fanout.size(); ++i) // iterate all fanout
            {
               // cout << i << endl;
               if(p->_fanout[i].get_gate() != 0) // not point to empty
               {
                  // cout << count << " keep moving" << endl;
                  printFanoutGate(count + 1, level, p->_fanout[i].isInv(), p->_fanout[i].gate());
               }
            }
         }
      }
      else
      {
         p->_ref = p->_global_ref;
         string _end = (count >= level && p->_fanout.size() != 0) ? "" : " (*)" ; 
         cout << s << _inv << s_type << p->id() << _end << endl;
      }
   }

   

}


