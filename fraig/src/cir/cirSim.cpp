/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions


/*******************************/
/*   Global variable and enum  */
/*******************************/
typedef pair<CirGate*, bool> GateV;
typedef unordered_map<size_t, vector<GateV> > FECHash;
typedef pair<size_t, vector<GateV> > FECPair;
typedef vector<vector<GateV> > FecGroups;

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
   // 1. First, generate random size_ts based on the number of pi for simulation
   // Hint : maybe using some random number generator to do this  
   // 2. Create a vector for accomodate FEC_Groups 
   // 3. Do simulation based on dfsList order (the entire gate)
   // Hint : 1. implement sym64 operation for aig gate (should has sym64 member in all gate)
   //        2. we first create a hash table, where key is the simulation result, and the value is the new vector group
   //           , that is, we push a value in the hash, if there is no value before, create a new vector and push it to
   //           a new vector groups, else we can find that the value is a vector, so we can just push it into that vector
   // 4. For each groups in the vector, keep doing another simulation again
   // Hint : Each group is actually a smaller FEC_Groups, then we just follow step 3, where we split the vector into several groups
   //        and then push it into a new bigger FEC_Groups 
   // 5. We will terminate each function if the number of groups does not change again (FEC_Groups.size())
   vector<vector<GateV> > FECGroups;
   if(FEC_GROUPS.size() != 0)
   {
      FECGroups = FEC_GROUPS;
   }
   else
   {
      vector<GateV> FECInit;
      for(int i = 0; i < _dfsList.size(); ++i)
      {
         FECInit.push_back(GateV(_dfsList[i], 0));
      }
      FECGroups.push_back(FECInit);
   }
   size_t count = 0, ceil = 0, stop = 0;
   // determine
   while(true)
   {
      if(FECGroups.size() <= 5)
      {
         ceil = 10; break;
      }
      else 
      {
         ceil = 10 + 300000 / FECGroups.size(); break;
      }
   }
   if(ceil >= 100) ceil = 100;
   cerr << "Groups size: " << FECGroups.size();
   while(true)
   {
      vector<vector<GateV> > TmpGroups;
      ++count;
      size_t g_size = FECGroups.size();
      // vector<size_t> numArr;
      for(int i = 0; i < _I; i++) // generating random number for each PI
      {
         int a = rnGen(INT32_MAX);
         int b = rnGen(INT32_MAX);
         // ********check the below code
         size_t ran = ((size_t)a << 32) + (size_t)b;
         // numArr.push_back(ran);
         _totalList[i+1]->_sim64 = ran;
      }
      // run simulation
      for(int i = 0; i < _dfsList.size(); i++)
      {
         if(_dfsList[i]->type() == CONST_GATE) continue;
         _dfsList[i]->op_sim_64();
      }
      // cout << FECGroups.size() << endl;
      // iterate all groups to let classify groups
      for(int k = 0; k < FECGroups.size(); ++k) // for each FECGroup
      {
         FECHash FECgroup;
         FECHash::iterator it;
         // FECHash::iterator it2;
         vector<GateV>& grp = FECGroups[k];
         for(int i = 0; i < grp.size(); ++i)
         {
            CirGate* p = grp[i].first;
            if(p == 0) continue;
            if(p->type() == AIG_GATE)
            {
               // cout << p->get_sim64() << endl;
               it = FECgroup.find(p->get_sim64());
               if(it != FECgroup.end()) // have some group before
               {
                  // push it into that FEC group
                  it->second.push_back(GateV(p, 0)); // the same sign
               }
               else // not found, try find inverse
               {
                  it = FECgroup.find(p->get_inv_sim64());
                  if(it != FECgroup.end()) // its inverse is in some group
                  {
                     it->second.push_back(GateV(p, 1)); // the opposite sign
                  }
                  else // not ever in the group, create a new vector
                  {
                     vector<GateV> g;
                     g.push_back(GateV(p, 0));
                     size_t FECKey = p->get_sim64();
                     FECgroup.insert(FECPair(FECKey, g));
                  }
               }
            }
         }

         for(it = FECgroup.begin(); it != FECgroup.end(); ++it)
         {
            if(it->second.size() == 1) continue; // if only one element, then just discard it
            TmpGroups.push_back(it->second);
         }

      }
      
      // copy TmpGroups to FECGroups
      FECGroups = TmpGroups;

      // termination
      // if(g_size == FECGroups.size() || FECGroups.size() >= _dfsList.size()) 
      // cout << TmpGroups.size() << endl;
      int w = 1, num = g_size;
      while(num / 10)
      {
         ++w; num /= 10;
      }
      for(int j = 0; j < w; ++j) cerr << '\b';
      for(int j = 0; j < w; ++j) cerr << ' ';
      for(int j = 0; j < w; ++j) cerr << '\b';

      cerr << FECGroups.size();
      // cout << "Group size: " << g_size << endl;
      // cout << FECGroups.size() << endl;
      
      // !!!!!!!!!!!! should eliminate those group with only one element
      if(g_size == FECGroups.size())
      {
         ++stop;
         if(stop >= ceil || count >= 1000)
         {
            cout << endl;
            // the group size does not change, then do fraig
            cout << count * 64 << " patterns simulated." << endl;
            FEC_GROUPS = FECGroups;
            // for(int i = 0; i < FEC_GROUPS.size(); ++i)
            // {
            //    for(int j = 0; j < FEC_GROUPS[i].size(); ++j)
            //    {
            //       cout << FEC_GROUPS[i][j].first->id() << endl;
            //    }
            // }
            break;
         }      
      }
      else
      {
         stop = 0;
      }
   }
}

void
CirMgr::fileSim(ifstream& patternFile)
{
   // 1. Read the file from patterFile
   // 2. Each line may contains bits the same as the number of PI(like we have 10 PI, then we will have 10 bit in a line)
   // 3. Then we can collect each bit to form a size_t (64 lines) and then to do something simillar to randomSim
   // Hint : 1. We can use bitmasking like (OR 11111110) to change the last bit, and 11111110 = ~00000001 = ~(1 << i)(where i = 1)
   //        2. Note that we should initialize the size_t to be 0 to prevent the situation like we cannot form a complete size_t (maybe only 50 lines)
   
   // read file 
   size_t count = 0, pat = 0;
   vector<string> writeOut;
   vector<string> outputPattern;
   if(patternFile.is_open())
   {
      
      string line;
      for(int i = 0; i < _I; ++i)
      {
         _totalList[i+1]->set_sim64(0);
      }
      cout << "Group size:    " << FEC_GROUPS.size();
      while(getline(patternFile, line))
      {
         // cout << line << endl;
         if(line.length() != _I)
         {
            cout << endl << "pattern: " << line << " length(" << line.length() << ") does not match number of inputs(" << _I << ") in the circuit!!" << endl;
            return ;
         }
         writeOut.push_back(line);
         for(int i = 0; i < _I; ++i)
         {
            size_t num;
            if(line[i] == '1') num = (size_t)1;
            else if(line[i] == '0') num = (size_t)0;
            else
            {
               cout << "pattern: " << line << " contains a non-0/1 characters \'" << line[i] << "\'" << endl;
            }
            
            size_t a = _totalList[i+1]->get_sim64();
            size_t b = ((size_t)1 & num) << count; 
            // cout << b << endl;
            _totalList[i+1]->set_sim64(a + b);
         }
         ++count;
         

         if(count == 64)
         {
            ++pat;
            simulate();
            genOutputPattern(outputPattern);
            writeSimFile(writeOut, outputPattern);
            writeOut.clear();
            outputPattern.clear();
            count = 0;
            for(int i = 0; i < _I; ++i)
            {
               _totalList[i+1]->set_sim64(0);
            }
         }
         // cout << (size_t)num << endl;
      }

      // for boundary case, that is, count < 64
      simulate();
      genOutputPattern(outputPattern);
      writeSimFile(writeOut, outputPattern);
      // cout << count << endl;
      cout << endl;
      patternFile.close();
      cout << pat * 64 + count << " patterns simulated." << endl;
   }
   
   
   
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/

void 
CirMgr::simulate()
{
   vector<vector<GateV> > FECGroups;   
   vector<vector<GateV> > TmpGroups;
   // initialization
   if(FEC_GROUPS.size() != 0)
   {
      FECGroups = FEC_GROUPS;
   }
   else
   {
      vector<GateV> FECInit;
      for(int i = 0; i < _dfsList.size(); ++i)
      {
         if(_dfsList[i] == 0) continue;
         if(_dfsList[i]->type() == AIG_GATE)
         {
             FECInit.push_back(GateV(_dfsList[i], 0));
         }
         
      }
      FECGroups.push_back(FECInit);
   }
   
   // run simulation
   for(int i = 0; i < _dfsList.size(); i++)
   {
      if(_dfsList[i]->type() != AIG_GATE) continue;
      _dfsList[i]->op_sim_64();
   }

   // iterate all groups to let classify groups
   for(int k = 0; k < FECGroups.size(); ++k) // for each FECGroup
   {
      FECHash FECgroup;
      FECHash::iterator it;
      // FECHash::iterator it2;
      vector<GateV>& grp = FECGroups[k];
      for(int i = 0; i < grp.size(); ++i)
      {
         CirGate* p = grp[i].first;
         if(p == 0) continue;
         if(p->type() == AIG_GATE)
         {
            // cout << p->get_sim64() << endl;
            it = FECgroup.find(p->get_sim64());
            if(it != FECgroup.end()) // have some group before
            {
               // push it into that FEC group
               it->second.push_back(GateV(p, 0)); // the same sign
            }
            else // not found, try find inverse
            {
               it = FECgroup.find(p->get_inv_sim64());
               if(it != FECgroup.end()) // has its inverse in a group
               {
                  it->second.push_back(GateV(p, 1)); // the opposite sign
               }
               else // not ever in the group, create a new vector
               {
                  vector<GateV> g;
                  g.push_back(GateV(p, 0));
                  size_t FECKey = p->get_sim64();
                  FECgroup.insert(FECPair(FECKey, g));
               }
            }
         }
      }

      for(it = FECgroup.begin(); it != FECgroup.end(); ++it)
      {
         if(it->second.size() == 1) continue; // if only one element, then just discard it
         TmpGroups.push_back(it->second);
      }
   }
   FEC_GROUPS = TmpGroups;
   int w = 1, num = FEC_GROUPS.size();
   while(num / 10)
   {
      ++w; num /= 10;
   }
   for(int j = 0; j < w; ++j) cerr << '\b';
   for(int j = 0; j < w; ++j) cerr << ' ';
   for(int j = 0; j < w; ++j) cerr << '\b';
   cerr << FEC_GROUPS.size();
}

void 
CirMgr::genOutputPattern(vector<string>& o)
{
   string s = "";
   set<size_t>::iterator it;
   for(int i = 0; i < 64; ++i)
   {
      s = "";
      for(it = _po.begin(); it != _po.end(); ++it)
      {
         bool a = _totalList[*it]->_sim64 & (1 << i);
         string b = a ? "1" : "0";
         s += b;
      }
      o.push_back(s);
   }
}

void
CirMgr::writeSimFile(vector<string>& w, vector<string>& o)
{
   for(int i = 0; i < w.size(); ++i)
   {
      *_simLog << w[i] << " " << o[i] << endl;
   }
   
}