/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp
bool
CmdParser::openDofile(const string& dof)
{
   // TODO...
   if(_dofile != 0) { // there are some files open 
      _dofileStack.push(_dofile);
      _dofile = new ifstream(dof.c_str());

      if(!(*_dofile)) { // cannot open file 
         _dofile = _dofileStack.top();
         _dofileStack.pop();
         return false;

      } else {
         return true;
      }
   } else {
      _dofile = new ifstream(dof.c_str());
      if(!(*_dofile)) { // cannot open file 
         _dofile = 0;
         return false;
      } else {
         return true;
      }
   }

   // !(*_dofile) // check if new file is not open
   // then
   

}

// Must make sure _dofile != 0
void
CmdParser::closeDofile()
{
   assert(_dofile != 0);
   // TODO...
   _dofile->close();
   delete _dofile;
   if(_dofileStack.size() != 0) {
      _dofile = _dofileStack.top();
      _dofileStack.pop();
   } else {
      _dofile = 0; 
   }
}

// Return false if registration fails
bool
CmdParser::regCmd(const string& cmd, unsigned nCmp, CmdExec* e)
{
   // Make sure cmd hasn't been registered and won't cause ambiguity
   string str = cmd;
   unsigned s = str.size();
   if (s < nCmp) return false;
   while (true) {
      if (getCmd(str)) return false;
      if (s == nCmp) break;
      str.resize(--s);
   }

   // Change the first nCmp characters to upper case to facilitate
   //    case-insensitive comparison later.
   // The strings stored in _cmdMap are all upper case
   //
   assert(str.size() == nCmp);  // str is now mandCmd
   string& mandCmd = str;
   for (unsigned i = 0; i < nCmp; ++i)
      mandCmd[i] = toupper(mandCmd[i]);
   string optCmd = cmd.substr(nCmp);
   assert(e != 0);
   e->setOptCmd(optCmd);

   // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
   return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// read in the command // may have error // not the default
// bool
// CmdParser::readCmd(istream& is)
// {
//    char c;
//    is.get(c);
//    if(c != '\0') return true; // have some command
//    else return false;         // have no command
// }

// Return false on "quit" or if exception happens
CmdExecStatus
CmdParser::execOneCmd()
{
   bool newCmd = false;
   if (_dofile != 0)
      newCmd = readCmd(*_dofile);
   else
      newCmd = readCmd(cin);

   // execute the command
   if (newCmd) {
      string option;
      CmdExec* e = parseCmd(option);
      if (e != 0)
         return e->exec(option);
      else 
         cout << "No command found!!!" << endl;;
   }
   return CMD_EXEC_NOP;
}

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void
CmdParser::printHelps() const
{
   CmdMap::const_iterator iter;

   for(iter = _cmdMap.begin(); iter != _cmdMap.end(); ++iter) {
      iter->second->help();
   }
}

void
CmdParser::printHistory(int nPrint) const
{
   assert(_tempCmdStored == false);
   if (_history.empty()) {
      cout << "Empty command history!!" << endl;
      return;
   }
   int s = _history.size();
   if ((nPrint < 0) || (nPrint > s))
      nPrint = s;
   for (int i = s - nPrint; i < s; ++i)
      cout << "   " << i << ": " << _history[i] << endl;
}


//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap
//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.
// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"
//
CmdExec*
CmdParser::parseCmd(string& option)
{
   assert(_tempCmdStored == false);
   assert(!_history.empty());
   string str = _history.back();

   // TODO...
   assert(str[0] != 0 && str[0] != ' ');

   string cmd = "";
   size_t pos = 0, end = 0;
   myStrGetTok(str, cmd, pos); // get the main cmd and return the pos of the token

   option = str.substr(cmd.length()); // remove the main cmd from option

   if(option != "") {
      option = option.substr(1);
   }
   
   return getCmd(cmd);


   // return NULL;
}

// Remove this function for TODO...
//
// This function is called by pressing 'Tab'.
// It is to list the partially matched commands.
// "str" is the partial string before current cursor position. It can be 
// a null string, or begin with ' '. The beginning ' ' will be ignored.
//
// Several possibilities after pressing 'Tab'
// (Let $ be the cursor position)
// 1. LIST ALL COMMANDS(DONE)
//    --- 1.1 ---
//    [Before] Null cmd
//    cmd> $
//    --- 1.2 ---
//    [Before] Cmd with ' ' only
//    cmd>     $
//    [After Tab]
//    ==> List all the commands, each command is printed out by:
//           cout << setw(12) << left << cmd;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location (including ' ')
//
// 2. LIST ALL PARTIALLY MATCHED COMMANDS(DONE)
//    --- 2.1 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$                   // partially matched
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$                   // and then re-print the partial command
//    --- 2.2 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$llo                // partially matched with trailing characters
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$llo                // and then re-print the partial command
//
// 3. LIST THE SINGLY MATCHED COMMAND(DONE)
//    ==> In either of the following cases, print out cmd + ' '
//    ==> and reset _tabPressCount to 0
//    --- 3.1 ---
//    [Before] partially matched (single match)
//    cmd> he$
//    [After Tab]
//    cmd> heLp $               // auto completed with a space inserted
//    --- 3.2 ---
//    [Before] partially matched with trailing characters (single match)
//    cmd> he$ahah
//    [After Tab]
//    cmd> heLp $ahaha
//    ==> Automatically complete on the same line
//    ==> The auto-expanded part follow the strings stored in cmd map and
//        cmd->_optCmd. Insert a space after "heLp"
//    --- 3.3 ---
//    [Before] fully matched (cursor right behind cmd)
//    cmd> hElP$sdf
//    [After Tab]
//    cmd> hElP $sdf            // a space character is inserted
//
// 4. NO MATCH IN FITST WORD(DONE)
//    --- 4.1 ---
//    [Before] No match
//    cmd> hek$
//    [After Tab]
//    ==> Beep and stay in the same location
//
// 5. FIRST WORD ALREADY MATCHED ON FIRST TAB PRESSING
//    --- 5.1 ---
//    [Before] Already matched on first tab pressing
//    cmd> help asd$gh
//    [After] Print out the usage for the already matched command
//    Usage: HELp [(string cmd)]
//    cmd> help asd$gh
//
// 6. FIRST WORD ALREADY MATCHED ON SECOND AND LATER TAB PRESSING
//    ==> Note: command usage has been printed under first tab press
//    ==> Check the word the cursor is at; get the prefix before the cursor
//    ==> So, this is to list the file names under current directory that
//        match the prefix
//    ==> List all the matched file names alphabetically by:
//           cout << setw(16) << left << fileName;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location
//    Considering the following cases in which prefix is empty:
//    --- 6.1.1 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and they do not have a common prefix,
//    cmd> help $sdfgh
//    [After] print all the file names
//    .               ..              Homework_3.docx Homework_3.pdf  Makefile
//    MustExist.txt   MustRemove.txt  bin             dofiles         include
//    lib             mydb            ref             src             testdb
//    cmd> help $sdfgh
//    --- 6.1.2 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and all of them have a common prefix,
//    cmd> help $orld
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    // e.g. in hw3/ref
//    cmd> help mydb-$orld
//    ==> DO NOT print the matched files
//    ==> If "tab" is pressed again, see 6.2
//    --- 6.1.3 ---
//    [Before] if prefix is empty, and only one file in the current directory
//    cmd> help $ydb
//    [After] print out the single file name followed by a ' '
//    // e.g. in hw3/bin
//    cmd> help mydb $
//    ==> If "tab" is pressed again, make a beep sound and DO NOT re-print 
//        the singly-matched file
//    --- 6.2 ---
//    [Before] with a prefix and with mutiple matched files
//    cmd> help M$Donald
//    [After]
//    Makefile        MustExist.txt   MustRemove.txt
//    cmd> help M$Donald
//    --- 6.3 ---
//    [Before] with a prefix and with mutiple matched files,
//             and these matched files have a common prefix
//    cmd> help Mu$k
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    ==> DO NOT print the matched files
//    cmd> help Must$k
//    --- 6.4 ---
//    [Before] with a prefix and with a singly matched file
//    cmd> help MustE$aa
//    [After] insert the remaining of the matched file name followed by a ' '
//    cmd> help MustExist.txt $aa
//    ==> If "tab" is pressed again, make a beep sound and DO NOT re-print 
//        the singly-matched file
//    --- 6.5 ---
//    [Before] with a prefix and NO matched file
//    cmd> help Ye$kk
//    [After] beep and stay in the same location
//    cmd> help Ye$kk
//
//    [Note] The counting of tab press is reset after "newline" is entered.
//
// 7. FIRST WORD NO MATCH(DONE)
//    --- 7.1 ---
//    [Before] Cursor NOT on the first word and NOT matched command
//    cmd> he haha$kk
//    [After Tab]
//    ==> Beep and stay in the same location
void
CmdParser::listCmd(const string& str)
{
   // TODO...
   CmdMap::iterator iter;
   int count = 0;
   int spaces = 0;
   // specially for counting spaces
   for(int i = 0; i < str.length(); ++i) { 
      if(str[i] == ' ') ++spaces;
   }
   // find how many options the str has
   vector<string> options;
   string option;
   size_t n = myStrGetTok(str, option);
   while (option.size()) {
      options.push_back(option);
      n = myStrGetTok(str, option, n);
   }
   
   if(options.size() == 0) // for "" and "     "(all spaces)
   { 
      cout << endl;
      for(iter = _cmdMap.begin(); iter != _cmdMap.end(); ++iter) {
         cout << setw(12) << left << (iter->first + iter->second->getOptCmd());
         ++count;
         if(count >= 5) {
            cout << endl;
            count = 0;
         } 
         // else {
         //    cout << ' ';
         // }
      }
      cout << endl;
      resetBufAndPrintPrompt(); // reset cmd
      // if "" then spaces = 0, if "    ", then insert spaces
      if(spaces > 0) insertChar(' ', spaces);
   } 
   else if(options.size() == 1) // for only one command, like "he"
   {
      // handle the case that with options and empty prefix
      if(_readBufPtr != _readBuf && *(_readBufPtr-1) == ' ') // case 6-1
      {
         if(_tabPressCount > 1)
         {
            vector<string> files;
            int count = 0;
            listDir(files, "", ".");
            vector<string> match_files;
            vector<string> tmp_files;
            //for(int i = 0)
            if(files.size() == 0) 
            {
               mybeep();
            }
            else if(files.size() == 1 && files[0].length() > 0) 
            {
               for(int i = 0; i < files[0].length(); ++i)
               {
                  insertChar(files[0][i]);
               }
               insertChar(' ');
            }
            else 
            {
               // looping through all files to find if some files have the same prefix
               string s = "";
               int num = 0, len = INT_MAX;
               char word;
               bool break_flag = false;
               for(int i = 0; i < files.size(); ++i)
               {
                  if(files[i].length() < len)
                  {
                     len = files[i].length();
                  }
               }
               for(int k = 0; k < len; ++k) // keep looping until the shortest file ends
               {
                  word = files[0][k];
                  for(int i = 0; i < files.size(); ++i)
                  {
                     if(files[i][k] != word)
                     {
                        break_flag = true;
                     }
                     if(break_flag) break;
                  }
                  if(break_flag) break;
                  s += word;
               }
               if(s != "")
               {
                  for(int i = 0; i < s.length(); ++i)
                  {
                     insertChar(s[i]);
                  }
                  insertChar(' ');
                  _tabPressCount = 0;
               }
               else 
               {
                  bool count_flag = true;
                  cout << endl;
                  string tmp = _readBuf;
                  for(int i = 0; i < files.size(); ++i)
                  {
                     cout << setw(16) << left << files[i];
                     ++count;
                     count_flag = false;
                     if(count >= 5)
                     {
                        cout << endl;
                        count = 0;
                        count_flag = true;
                     }
                  }
                  if(!count_flag) cout << endl;
                  resetBufAndPrintPrompt();
                  for(int i = 0; i < tmp.size(); i++)
                  {
                     insertChar(tmp[i]);
                  }
               }
            }
         }
         return;
      }
      // ===================================================
      vector<string> opt_cmds;

      // keep iterating until we screen out all impossible cmds
      // */=====================================================
      for(iter = _cmdMap.begin(); iter != _cmdMap.end(); ++iter) {
         // if they match the commmand
         string opt_cmd = iter->first + iter->second->getOptCmd();
         if(!myStrNCmp(opt_cmd, str, str.length())) { 
            opt_cmds.push_back(opt_cmd);
         }
      }
      // =====================================================*/
      // print selected cmd
      if(opt_cmds.size() == 0) 
      {
         mybeep();
      }
      else if(opt_cmds.size() == 1) // find the unique command for matching
      { 
         string s = opt_cmds[0];
         for(int i = str.length(); i < s.length(); i++) 
         {
            insertChar(s[i]);
         }
         insertChar(' ');
      } 
      else if(opt_cmds.size() > 1) // contains
      {
         // print all possible commands
         // */===========================
         bool count_flag = false;
         cout << endl;
         for(int i = 0; i < opt_cmds.size(); ++i) 
         {
            cout << setw(12) << left << opt_cmds[i];
            count_flag = false;
            if(count >= 5) 
            {
               cout << endl;
               count = 0;
               count_flag = true;
            } 
            // else 
            // {
            //    cout << ' ';
            // }
         }
         // if we have meet count == 5, then we shall not cout << endl again
         if(!count_flag) cout << endl;
         // ===========================*/
         // insert the missing words after cursor
         // /*======================================
         char* tmpPtr = _readBufPtr;
         string tptr = _readBufPtr;
         string tmp = _readBuf;
         int diff = tmp.length() - tptr.length();
         resetBufAndPrintPrompt();
         for(int i = 0; i < tmp.length(); ++i) {
            insertChar(tmp[i]);
         }
         moveBufPtr(tmpPtr);
         for(int i = diff; i < str.length(); ++i) {
            insertChar(str[i]);
         }
         // ======================================*/
      } 
   } 
   else // for two commands and up, like he hahaha
   {
      option = options[0]; // the main cmd
      vector<CmdRegPair> opt_cmds;
      for(iter = _cmdMap.begin(); iter != _cmdMap.end(); ++iter)
      {
         string opt_cmd = iter->first + iter->second->getOptCmd();
         if(!myStrNCmp(opt_cmd, option, option.length())) // find a matching cmd
         {
            CmdRegPair match_opt(iter->first, iter->second);
            opt_cmds.push_back(match_opt);
         }
      }
      if(opt_cmds.size() == 0) 
      {
         mybeep();
      }
      else if(opt_cmds.size() == 1 && _tabPressCount <= 1) // only one command matching
      {
         cout << endl;
         string tmp = _readBuf;
         opt_cmds[0].second->usage(cout); // print the usage of the cmd
         int tab_count = _tabPressCount;
         resetBufAndPrintPrompt();
         _tabPressCount = tab_count;
         for(int i = 0; i < tmp.size(); i++)
         {
            insertChar(tmp[i]);
         }
      }
      else if(_tabPressCount > 1)
      {
         string tmp = "";
         string s = "";
         for(int i = str.length() - 1; i >= 0; --i)
         {
            if(str[i] != ' ') tmp += str[i];
            else break;
         }
         for(int i = tmp.length() - 1; i >= 0; --i)
         {
            s += tmp[i];
         }
         vector<string> files;
         int count = 0;
         listDir(files, s, ".");
         vector<string> match_files;
         vector<string> tmp_files;
         // not find a suitable prefix
         if(files.size() == 0) 
         {
            mybeep();
         }
         else if(files.size() == 1 && files[0].length() > 0) 
         {
            for(int i = str.length(); i < files[0].length(); ++i)
            {
               insertChar(files[0][i]);
            }
            insertChar(' ');
         }
         else 
         {
            // looping through all files to find if some files have the same prefix
            string s = "";
            int len = INT_MAX;
            char word;
            bool break_flag = false;
            for(int i = 0; i < files.size(); ++i)
            {
               if(files[i].length() < len)
               {
                  len = files[i].length();
               }
            }
            for(int k = 0; k < len; ++k) // keep looping until the shortest file ends
            {
               word = files[0][k];
               for(int i = 0; i < files.size(); ++i)
               {
                  if(files[i][k] != word)
                  {
                     break_flag = true;
                  }
                  if(break_flag) break;
               }
               if(break_flag) break;
               s += word;
            }
            if(s != "")
            {
               for(int i = str.length(); i < s.length(); ++i)
               {
                  insertChar(s[i]);
               }
               insertChar(' ');
            }
            else 
            {
               bool count_flag = true;
               cout << endl;
               string tmp = _readBuf;
               for(int i = 0; i < files.size(); ++i)
               {
                  cout << setw(16) << left << files[i];
                  ++count;
                  count_flag = false;
                  if(count >= 5)
                  {
                     cout << endl;
                     count = 0;
                     count_flag = true;
                  }
               }
               if(!count_flag) cout << endl;
               resetBufAndPrintPrompt();
               for(int i = 0; i < tmp.size(); i++)
               {
                  insertChar(tmp[i]);
               }
            }
         }
      
      }
      
   }
}

// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
//
CmdExec*
CmdParser::getCmd(string cmd)
{
   CmdExec* e = 0;
   // TODO...
   CmdMap::iterator iter;

   for(iter = _cmdMap.begin(); iter != _cmdMap.end(); ++iter) {
      if(!(myStrNCmp(iter->first + iter->second->getOptCmd(), cmd, iter->first.length()))){ // find the command
         e = iter->second;
      }
   }

   // if(e == 0) cerr << "No command found!";
   
   return e;
}


//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// return false if option contains an token
bool
CmdExec::lexNoOption(const string& option) const
{
   string err;
   myStrGetTok(option, err);
   if (err.size()) {
      errorOption(CMD_OPT_EXTRA, err);
      return false;
   }
   return true;
}

// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true
//
bool
CmdExec::lexSingleOption
(const string& option, string& token, bool optional) const
{
   size_t n = myStrGetTok(option, token);
   if (!optional) {
      if (token.size() == 0) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
   }
   if (n != string::npos) {
      errorOption(CMD_OPT_EXTRA, option.substr(n));
      return false;
   }
   return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool
CmdExec::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
      if (tokens.size() > nOpts) {
         errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
         return false;
      }
   }
   return true;
}

CmdExecStatus
CmdExec::errorOption(CmdOptionError err, const string& opt) const
{
   switch (err) {
      case CMD_OPT_MISSING:
         cerr << "Error: Missing option";
         if (opt.size()) cerr << " after (" << opt << ")";
         cerr << "!!" << endl;
      break;
      case CMD_OPT_EXTRA:
         cerr << "Error: Extra option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_ILLEGAL:
         cerr << "Error: Illegal option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_FOPEN_FAIL:
         cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
      break;
      default:
         cerr << "Error: Unknown option error type!! (" << err << ")" << endl;
      exit(-1);
   }
   return CMD_EXEC_ERROR;
}

