/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <iomanip>
#include "p2Json.h"

using namespace std;

// Implement member functions of class Row and Table here
void JsonElem::print()
{
   cout << _key << " : " << _value;
}

void JsonElem::setKey(string &key)
{
   _key = key;
}

void JsonElem::setValue(int &value)
{
   _value = value;
}

bool Json::read(const string &inputFile)
{
   fstream file;
   string line;
   string word = "";
   int key_flag = 0;
   int num_flag = 0;
   vector<string> keys;
   vector<string> values;
   file.open(inputFile, ios::in);
   if (!file)
   {
      return false;
   }
   else
   {
      // read file line by line
      if (file.is_open())
      {
         while (getline(file, line))
         {
            for (int i = 0; i < line.length(); i++)
            {
               int a = int(line[i]);
               if (line[i] == '{' || line[i] == '}')
               {
                  break;
               }
               else if (line[i] == '\"' && key_flag == 0)
               {
                  word += line[i];
                  key_flag = 1;
               }
               else if (line[i] != '\"' && key_flag == 1)
               {
                  word += line[i];
               }
               else if (line[i] == '\"' && key_flag == 1)
               {
                  word += line[i];
                  keys.push_back(word);
                  word = "";
                  key_flag = 0;
               }
               else if (a == 45 && num_flag == 0) // -(minus) symbol
               {
                  word += line[i];
               }
               else if (a >= 48 && a <= 57 && num_flag == 0)
               { // ascii for 0~9
                  num_flag = 1;
                  word += line[i];
               }
               else if (a >= 48 && a <= 57 && num_flag == 1)
               { // keep recording the number(value)
                  word += line[i];
               }
               else if ((a < 48 || a > 57) && num_flag == 1) // stop counting the number
               {                                             // not in 0~9
                  //word = string_to_int(word); // convert string to integer
                  values.push_back(word);
                  word = "";
                  num_flag = 0;
               }
            }
         }
         file.close();
      }
   }
   if (word.length() != 0) // the last element of the json file
   {
      // cout << word << endl;
      //word = string_to_int(word); // convert string to int
      values.push_back(word);
      // string key = words[0];
   }
   for (int i = 0; i < keys.size(); i++)
   {
      string input = "ADD " + keys[i] + ' ' + values[i];
      ADD(input);
   }
   return true;
}

int Json::string_to_int(string &snum)
{
   int num = 0;
   int power = 1;
   if (snum[0] == '-') // negative number
   {
      for (int i = snum.length() - 1; i >= 1; i--)
      {
         num += ((int)snum[i] - 48) * power;
         power *= 10;
      }
      num = -num;
   }
   else // positive number
   {
      for (int i = snum.length() - 1; i >= 0; i--)
      {
         num += ((int)snum[i] - 48) * power;
         power *= 10;
      }
   }

   return num;
}

void Json::PRINT()
{
   if (_obj.size() != 0)
   {
      cout << '{' << endl;
      for (int i = 0; i < _obj.size() - 1; ++i)
      {
         cout << "  ";
         _obj[i].print();
         cout << ',' << endl;
      }
      cout << "  ";
      _obj[_obj.size() - 1].print();
      cout << "\n}\n";
   }
   else
   {
      cout << '{' << '\n'
           << '}' << endl;
   }
}

void Json::ADD(string &line)
{
   string word = "";
   vector<string> add_command;

   for (int i = 4; i < line.length(); i++) // count from key(the 4th char) --- (add )....
   {
      if (line[i] == ' ')
      {
         add_command.push_back(word);
         word = "";
      }
      else
      {
         word = word + line[i];
      }
   }

   if (word.length() > 0)
      add_command.push_back(word);

   // add_command[0] is "ADD"
   string key = add_command[0];
   // cout << add_command[1] << endl;
   int value = string_to_int(add_command[1]);

   // push a JsonElem with key-value pair
   _obj.push_back(JsonElem(key, value));

   // find length(how many JsonElem), sum and average
   length++;
   sum += value;
   // ave = fixed << setprecision(1) << (float)sum / (float)length;

   // determine max and min element
   if (length == 1)
   {
      max = _obj[0];
      min = _obj[0];
   }
   else
   {

      if (max.getValue() < value)
      {
         max.setKey(key);
         max.setValue(value);
      }

      if (min.getValue() > value)
      {
         min.setKey(key);
         min.setValue(value);
      }
   }
}

ostream &
operator<<(ostream &os, const JsonElem &j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}
