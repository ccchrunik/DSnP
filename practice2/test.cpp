#include <stdio.h>
#include <iostream>

using namespace std;

int main() {
    
    int i;
    const int j = i;
    const int& k = i;
    cout << i << ' ' << j << ' ' << k << endl;
    i = 10;
    cout << i << ' ' << j << ' ' << k << endl;

    return 0;
}