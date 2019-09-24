#include <stdio.h>
#include <iostream>

using namespace std;
// putting the default value at delaration, this can assing value if no argument is puet in
void f(int a = 10);

int main() {
    // int a = 10;
    f();
    return 0;
}

void f(int a)
{
    cout << a << endl;
}



