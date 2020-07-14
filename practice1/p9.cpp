#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

class B {
    public:
        B() {

        }
    friend class A;
    private:
        int _bd;
};

class A {
    public:
        A() {
            _b._bd = 5;
            cout << _b._bd << endl;
        }
    private:
        int _bd;
        B _b;
};


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
  
    A a1;
    B b1;

    
    return 0;
}




