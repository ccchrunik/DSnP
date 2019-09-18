#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

void f() {
    int a, b, c;
    cout << &a << endl;
    cout << &b << endl;
    cout << &c << endl;
    int *p = new int;
    int *q = new int;

    cout << &p << endl;
    cout << &q << endl;
    cout << endl;
}

int main() {
    
    f();
    f();
    f();

    
    return 0;
}




