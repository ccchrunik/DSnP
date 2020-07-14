#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

class C {
    public:
        C() {
            _a = 10;
            _b = 20;
        }

        void printAddr() const {
            cout << &_a << endl;
            cout << &_b << endl;
        } 

        void printData() const {
            cout << _a << endl;
            cout << _b << endl;
        }
    private: 
        int _a;
        int _b;
};

int main() {
    // int t = (1 << 31);
    
    C c1;
    C c2;

    c1.printAddr();
    c2.printAddr();

    c1.printData();
    c2.printData();


    
    return 0;
}




