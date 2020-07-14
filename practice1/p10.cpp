#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

class C {
    friend class A;
};

class B {
    int _bd;
    friend class A;
};

class A {
    
    public:
        B _b;
        C *_c;
        void printAddr() {
            cout << &_b << endl;
            cout << &_b._bd << endl;
            cout << &_c << endl;
        }
};


int main() {
    // int t = (1 << 31);
  
    A a;
    cout << &a << endl;
    a.printAddr();
    cout << a._c << endl;
    
    return 0;
}




