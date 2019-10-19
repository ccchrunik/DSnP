/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   // not yet check, if not read file right
   string s;
   if(!(is >> s)) 
      return (is >> s);

   string key_words = "", value_words = "";
   char word;
   bool key_f = false, value_f = false;
   bool invalid_key = false;
   while(is.get(word)) {
      if(word == '\"' && !key_f) key_f = true; // start the key
      else if(word == '\"' && key_f) key_f = false; // end the key
      else if(word == ':' && !value_f) value_f = true; // start the value
      else if(word == ',' && value_f) {
         value_f = false; // end the value
         int value = stoi(value_words); // convert string to integer

         // add element in DBJson
         DBJsonElem elm(key_words, value); 
         j.add(elm);
         key_words = ""; value_words = "";
      }
      else if(((word >= 65 && word <= 90) || (word >= 97 && word <= 122) || word == '_') && key_f && key_words.length() == 0) { 
         // record the key if this is the first letter
         key_words += word; 
      } else if(((word >= 65 && word <= 90) || (word >= 97 && word <= 122) || (word >= 48 && word <= 57) || word == '_') && key_f && key_words.length() > 0) { 
         // record the key if this is not the first letter
         key_words += word; 
      } else if(word >= 48 && word <= 57 && value_f) { // record the value
         value_words += word;
      } else if(((word >= 65 && word <= 90) || (word >= 97 && word <= 122) || (word >= 48 && word <= 57) || word == '_') && key_f && key_words.length() == 0) {
         // if there is an invalid key, set it to trues
         invalid_key = true;
      }
   }

   // add the last element since it doesnt have ','
   if(key_words != "" && value_words != "") {
      
      value_f = false; // end the value
      int value = stoi(value_words); // convert string to integer

      // add element in DBJson
      DBJsonElem elm(key_words, value); 
      j.add(elm);
   }
   
   
   j.success = true; // read file successfully
   
   // may have error
   return is>>s;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   os << '{' << endl;
   if(j.size() > 0) {
      for(int i = 0; i < j.size() - 1; ++i) { 
         os << "  " << j[i] << ',' << endl;
      }
      os << "  " << j[j.size() - 1] << endl;
   }
   os << '}';
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
   // I think it is to clear all value in DBJson
   _obj.clear();
   MAX = 0.0; MIN = 0.0; SUM = 0.0; AVE = 0.0; success = false;
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
   // check if the key is repeated
   // if(!isValidVarName(elm.key())) return false; // not a valid key name
   if(size() > 0) {
      for(int i = 0; i < size(); ++i) {
         if(elm.key() == _obj[i].key()) {
            return false;
         }
      }
   }
   _obj.push_back(elm); // add element into DBJson
   if(size() == 1) {
      MAX = elm.value();
      max_index = 0;
      MIN = elm.value();
      min_index = 0;
   } else {
      if(elm.value() > MAX) {
         MAX = elm.value();         // reset MAX
         max_index = size() - 1;
      }
      else if(elm.value() < MIN) {
         MIN = elm.value();    // reset MIN
         min_index = size() - 1;
      }
   }
   

   SUM = SUM + elm.value();        // reset SUM
   AVE = (float)SUM / (float)size();

   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   if(empty()) return NAN;  // may have error empty case
   else return AVE;         // normal case
}

// If DBJson is empty, set idx to size() and return INT_MAX
int
DBJson::max(size_t& idx) const
{
   // TODO
   if(empty()) {
      idx = size();
      return INT_MAX;  // empty case
   } 
   else {
      idx = max_index;
      return MAX;            // normal case
   }

}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
   // TODO
   if(empty()) {
      idx = size();
      return INT_MIN; // empty case
   }
   else {
      idx = min_index;
      return MIN;            // normal case
   }

}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
   if(size() > 0) {
      int max_v = _obj[0].value(), min_v = _obj[0].value();
      int max_key = 0, min_key = 0;
      for(int i = 0; i < size(); ++i) {
         if(_obj[i].value() > max_v) {
            max_key = i;
            max_v = _obj[i].value();
         }
         if(_obj[i].value() < min_v) {
            min_key = i;
            min_v = _obj[i].value();
         }
      }
      
      MAX = max_v;
      max_index = max_key;
      MIN = min_v;
      min_index = min_key;

   }
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
   if(size() > 0) {
      MAX = _obj[size() - 1].value();
      max_index = size() - 1;
      MIN = _obj[0].value();
      min_index = 0;
   }
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   if(empty()) return 0;  // empty case
   else return SUM;       // normal case
}

vector<DBJsonElem>
DBJson::find_key(string& key) const
{
   vector<DBJsonElem> match_key;
   bool fail = false;
   for(int i = 0; i < size(); ++i) {
      if(!myStrNCmp(_obj[i].key(), key, key.length())) { // if there is an command
         match_key.push_back(_obj[i]); // push key to the vector
      }
   }
   
   DBJsonElem f("", 0);
   
   if(match_key.size() == 0) {
      match_key.push_back(f);
   }

   return match_key;
}
