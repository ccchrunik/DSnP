/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"
#include <vector>

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   // parsed option 
   vector<string> options;
   string str = option;
   string opt = "";
   size_t n = myStrGetTok(str, opt);
   while (opt.size()) {
      options.push_back(opt);      // push new parsed string into options
      n = myStrGetTok(str, opt, n);
   }

   if(options.size() == 1)                          // create objs, not arrays
   { 
      // Use try-catch to catch the bad_alloc exception
      try {
         int num = 0;
         if(!myStr2Int(options[0], num)) // convert string to number, if not a number, return error
         {
            cerr << "String to number Error!" << endl;
            return CMD_EXEC_ERROR;
         }

         if(num <= 0) // num should > 0
         {
            cerr << "Number out of range!" << endl;         
            return CMD_EXEC_ERROR;
         }
         mtest.newObjs(num);         // create objs

      } catch(std::bad_alloc& ba) {  // if some allocation happens, catch bad_alloc
         cerr << "bad_alloc caught: " << "The size of memory block is smaller than required." << endl;
      }
   } 
   else if(options.size() == 3)// options.size() = 3, create arrays 
   {
      int num = 0, size = 0, index = 0;
      bool isArray = false, isNum = false;
      for(int i = 0; i < 2; ++i)
      {
         if(options[i].length() >= 2 && !myStrNCmp("-Array", options[i], 2)) // if option is recognizable
         {
            if(!myStr2Int(options[i+1], size)) // if next option is not a number
            {
               cerr << "String to number Error!" << endl;
               return CMD_EXEC_ERROR;
            }

            if(size <= 0)
            {
               cerr << "Size out of range!" << endl;
               return CMD_EXEC_ERROR;
            }

            // if above requests meet, then save index of the -Array keyword
            index = i;
            isArray = true;
            break;
         }  
      }
      
      // check the number of new times is a number
      if(index == 0) isNum = myStr2Int(options[2], num);
      else if(index == 1) isNum = myStr2Int(options[0], num);
      else isNum = false;

      // Use try-catch to catch the bad_alloc exception
      if(isArray && isNum)
      {
         try {
            if(num <= 0) // should > 0
            {
               cerr << "Number out of range!" << endl;
               return CMD_EXEC_ERROR;
            }
            mtest.newArrs(num, size);    // create arrays
            // cout << "fourth" << endl;
         } catch(std::bad_alloc& ba) {
            cerr << "bad_alloc caught: " << "The size of the memory block is smaller than required." << endl;
         }
      }
      else 
      {
         cerr << "Error Command!" << endl;
         return CMD_EXEC_ERROR;
      }
   }
   else 
   {
      cerr << "The number of parameter is worng!" << endl;
      return CMD_EXEC_ERROR;      
   }
   
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   // parse options
   vector<string> options;
   string str = option;
   string opt = "";
   size_t n = myStrGetTok(str, opt);
   while (opt.size()) {
      options.push_back(opt);
      n = myStrGetTok(str, opt, n);
   }

   // cout << option << endl;

   if(options.size() == 2) // maybe delete -i 3 or delete -r 5
   { 
      if(!(myStrNCmp("-Index", options[0], 2))) // options[1] == -index(-i)
      {
         int index;
         if(!myStr2Int(options[1], index)) // convert string to number, if not, return false
         {
            cerr << "String to Number Error" << endl;
            return CMD_EXEC_ERROR;
         }

         if(index > mtest.getObjListSize() - 1 || index < 0) // index out of range
         { 
            cerr << "Index out of range!" << endl;
            return CMD_EXEC_ERROR;
         } 
         else 
         {
            mtest.deleteObj(index);             // delete obj
         }
      } 
      else if(!(myStrNCmp("-Random", options[0], 2))) // options[1] == -random(-r)
      {
         int num;                       // num is the delete times
         if(!myStr2Int(options[1], num)) // convert string to numver, if not, return error
         {
            cerr << "String to Number Error!" << endl;
            return CMD_EXEC_ERROR;
         }

         if(num < 0)
         {
            cerr << "Number out of range" << endl;
            return CMD_EXEC_ERROR;
         }
         else 
         {
            int index;
            int range = mtest.getObjListSize();

            for(int i = 0; i < num; ++i) 
            {
               index = rnGen(mtest.getObjListSize());
               mtest.deleteObj(index);
            }
         } 
      } 
      else // options[0] is not a legal command, like -Array
      {
         cerr << "Some options are illegal!" << endl;
         return CMD_EXEC_ERROR;
      }
  
   } 
   else if(options.size() == 3) // delete array
   { 
      // find which option is -Array
      int arr_index = 0, opt_index = 0;
      if(options[0].length() >= 2 && !myStrNCmp("-Array", options[0], 2)) arr_index = 0;
      else if(options[2].length() >= 2 && !myStrNCmp("-Array", options[2], 2)) arr_index = 2;
      else 
      {
         cerr << "Some option is illegal!" << endl;
         return CMD_EXEC_ERROR;
      }

      if(arr_index == 0) opt_index = 1;
      else if(arr_index == 2) opt_index = 0; 

      if(options[opt_index].length() >= 2 && !(myStrNCmp("-Index", options[opt_index], 2))) // options[opt_index] == -index(-i)
      {
         int index = 0;
         if(!myStr2Int(options[opt_index+1], index)) // convert string to number
         {
            cerr << "String to Number Error" << endl;
            return CMD_EXEC_ERROR;
         }

         if(mtest.getArrListSize() <= 0) 
         {
            cerr << "Size of of range!" << endl;
            return CMD_EXEC_ERROR;
         }
            
         if(index > mtest.getArrListSize() - 1 || index < 0) // index out of range
         {  
            cerr << "Index out of Range!" << endl;
            return CMD_EXEC_ERROR;
         } 
         else 
         {
            mtest.deleteArr(index);
         }
      } 
      else if(options[opt_index].length() >= 2 && !myStrNCmp("-Random", options[opt_index],  2)) // options[opt_index] == -random(-r)
      {
         int num;                               // num is the delete times
         if(!myStr2Int(options[opt_index+1], num))
         {
            cerr << "String to Number Error" << endl;
            return CMD_EXEC_ERROR;
         } 

         if(mtest.getArrListSize() <= 0)
         {
            cerr << "Size out of range!" << endl;
            return CMD_EXEC_ERROR;
         }

         if(num < 0)
         {
            cerr << "Number out of range" << endl;
            return CMD_EXEC_ERROR;
         }
         else 
         {
            int index;
            int range = mtest.getArrListSize();


            for(int i = 0; i < num; ++i) // delete num times 
            {
               index = rnGen(mtest.getArrListSize());
               mtest.deleteArr(index);
            }
         } 
      } 
      else // options[opt_index] is illegal
      { 
         cerr << "The option is illegal!" << endl;
         return CMD_EXEC_ERROR;
      }

   } 
   else // option is illegal
   { 
      cerr << "The number of parameters is wrong!" << endl;
      return CMD_EXEC_ERROR;
   }


   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


