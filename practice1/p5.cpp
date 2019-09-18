#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

class C{
    int x;
    int y;
    int z;
};

int main() {
    // int t = (1 << 31);
    int *a1 = new int;
    int *a2 = new int;
    C *c1 = new C;
    C *c2 = new C;   
    C *c3 = new C;
    // cout << t << endl;
    cout << sizeof(C) << endl;
    cout << &a1 << endl;
    cout << &a2 << endl;
    cout << &c1 << endl;
    cout << &c2 << endl;
    cout << &c3 << endl;

    
    return 0;
}




