#include <stdio.h>
#include <iostream>

using namespace std;

int main() {
    
    const int a = 10;
    int b = 10;
    // can change the value of c by changing the value of b
    const int& c = b;  b = 20;

    cout << a << endl;
    cout << b << endl;
    cout << c << endl;
    

    return 0;
}