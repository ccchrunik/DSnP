#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main() {

    int a = 10;
    int &r = a;
    
    // cout << &a << endl;
    // cout << &r << endl;

    int b = 20;
    r = b;

    cout << &a << endl;
    cout << &b << endl;
    cout << &r << endl;
    
    cout << a << endl;
    cout << b << endl;
    cout << r << endl;
    

    return 0;
}




