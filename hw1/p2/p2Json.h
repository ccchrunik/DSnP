/****************************************************************************
  FileName     [ p2Json.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Json JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_JSON_H
#define P2_JSON_H

#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <unordered_set>

using namespace std;

class JsonElem
{
public:
   // TODO: define constructor & member functions on your own
   JsonElem() {};
   JsonElem(const string& k, int v): _key(k), _value(v) {};

   void print();
   string getKey() { return _key; };
   int getValue() { return _value; };
   void setKey(string &);
   void setValue(int &);

   friend ostream& operator << (ostream&, const JsonElem&);

private:
   string  _key;   // DO NOT change this definition. Use it to store key.
   int     _value; // DO NOT change this definition. Use it to store value.
};



class Json
{
public:
   // TODO: define constructor & member functions on your own
   bool read(const string&);
   int string_to_int(string&);
   void PRINT();
   void SUM() { cout << "The summation of the values is: " << sum << "." << endl; };
   void AVE() { cout << "The average of the values is: " << fixed << setprecision(1) << (float)sum / (float)length << "." << endl; };
   void MAX() { cout << "The maximum element is: { " << max.getKey() << " : " << max.getValue() << " }." << endl; };
   void MIN() { cout << "The maximum element is: { " << min.getKey() << " : " << min.getValue() << " }." << endl; };
   void ADD(string &);


private:
   vector<JsonElem>       _obj;  // DO NOT change this definition.
                                 // Use it to store JSON elements.
   int sum = 0, length = 0;
   // float ave = 0.0;
   JsonElem max, min;

};




#endif // P2_TABLE_H
