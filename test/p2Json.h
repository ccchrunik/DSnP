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
#include <unordered_set>

using namespace std;

class JsonElem
{
public:
   // TODO: define constructor & member functions on your own
   JsonElem() {}
   JsonElem(const string& k, int v): _key(k), _value(v) {}
   void print();
   string getKey() { return _key; };
   int getValue() { return _value; };
   friend ostream& operator << (ostream&, const JsonElem&);

private:
   string  _key;   // DO NOT change this definition. Use it to store key.
   int     _value; // DO NOT change this definition. Use it to store value.
};

void JsonElem::print() {
   cout << "  " << _key << ": " << _value;
}

class Json
{
public:
   // TODO: define constructor & member functions on your own
   bool read(const string&);
   void print();
   void SUM() { cout << "The summation of the values is: " << sum << "." << endl; };
   void AVE() { cout << "The average of the values is: " << ave << "." << endl; };
   void MAX() { cout << "The maximum element is: { " << max.getKey() << " : " << max.getValue() << " }." << endl; };
   void MIN() { cout << "The maximum element is: { " << min.getKey() << " : " << min.getValue() << " }." << endl; };
   void ADD();


private:
   vector<JsonElem>       _obj;  // DO NOT change this definition.
                                 // Use it to store JSON elements.
   int sum;
   float ave;
   JsonElem max, min;

};

bool Json::read(const string& inputFile) {
   fstream file;
   file.open(inputFile, ios::in);
   

}

void Json::print() {
    cout << '{' << endl;
    for(int i = 0; i < _obj.size() - 1; ++ i) {
       _obj[i].print();
       cout << ',' << endl;
    }
    _obj[_obj.size() - 1].print();
    cout << endl << '}';
}

void Json::ADD() {

}


#endif // P2_TABLE_H
