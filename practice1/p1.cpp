#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main() {

    int x, y, z;
    printf("%p\n", (void*)&x);
    printf("%p\n", (void*)&y);
    printf("%p\n", (void*)&z);
    
    int a, b, c;
    cout << "&a = " << &a << endl;
    cout << "&b = " << &b << endl;
    cout << "&c = " << &c << endl;
    cout << "&b = " << &b << endl;    
    cout << "&a = " << &a << endl;
    cout << "&c = " << &c << endl;

    // int *p = 10;
    // int *p = (int*)10;
    
    // int a = 10;
    // int b = 20;
    // int& i = a;
    // int j = i; // what are the values of a, b, i, j?
    // j = 30; // what are the values of a, b, i, j?
    // i = b; // what are the values of a, b, i, j?
    // cout << a << " " << b << " " << i << " " << j << endl;
    return 0;
}




