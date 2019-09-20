/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <stdio.h>
#include "p2Json.h"

using namespace std;

string to_upper(string &input)
{
    string comm = "";
    int a, n;
    bool command = false;
    for(int i = 0; i < input.length(); i++)
    {
        if(!command && input[i] != ' ' && input[i] >= 97 && input[i] <= 122) // let the command argument upper(if lower)
        {
            a = (int)input[i] - 32;
            comm += (char)a;
        }
        else if (!command && input[i] != ' ' && input[i] >= 97 && input[i] <= 122) // if command are already upper
        {
            comm += (char)a;
            
        }
        else if(!command && input[i] == ' ') // stop upper the input
        {
            comm += input[i];
            command = true;
        }
        else
        {
            comm += input[i];
        }
        
        
        
    }
    return comm;
}

vector<string> to_parse(string &line)
{
    string word = "";
    vector<string> words;
    for(int i = 0; i < line.length(); i++)
    {
        // cout << line[i] << " ";
        if(line[i] != ' ')
        {
            word += line[i];
        }
        else
        {
            // cout << word << ' ';
            words.push_back(word);
            word = "";
        }
        
    }
    if(word.length() > 0)
    {
        // cout << word << endl;
        words.push_back(word);
    }

    return words;
}

int main()
{
   Json json;

   // Read in the csv file. Do NOT change this part of code.
   string jsonFile;
   cout << "Please enter the file name: ";
   cin >> jsonFile;
   if (json.read(jsonFile))
      cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
      exit(-1); // jsonFile does not exist.
   }

   // TODO read and execute commands
   // cout << "Enter command: ";
   string buffer;
   getline(cin, buffer); // clear the buffer
   bool exit = false;
   while (true) {
       string input;
       
       string line;
       vector<string> comm;
       cout << "Enter command: ";
       // cout << '1' << endl;
       getline(cin, input);
       //cout << '1' << ' ';
       // make input string to upper case
       line = to_upper(input);
       // cout << line << endl;
       //cout << '2' << ' ';
       comm = to_parse(line);
       //cout << '3' << endl;
       // cout << comm[0] << endl;
       
       
       if (comm[0] == "ADD") {
           //cout << '4' << endl;
           // cout << comm[0] << " " << comm[1] << " " << comm[2] << endl;
           string send = comm[0] + " \"" + comm[1] + "\" " + comm[2];
           json.ADD(send);
       } else if (comm[0] == "PRINT") {
           json.PRINT();
       } else if (comm[0] == "SUM") {
           json.SUM();
       } else if (comm[0] == "AVE") {
           json.AVE();
       } else if (comm[0] == "MAX") {
           json.MAX();
       } else if (comm[0] == "MIN") {
           json.MIN();
       } else if (comm[0] == "EXIT") {
           exit = true;
           break;
       } else {
          cout << "Error command!" << endl;
       }
   }
}





