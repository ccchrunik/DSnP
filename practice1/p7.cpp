#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

class C {
    public:
        void printAddr() const {
            cout << &this->_a << endl;
            cout << &this->_b << endl;
            // cout << &_a << endl;
            // cout << &_b << endl;
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


    
    return 0;
}




