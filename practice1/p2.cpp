#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main() {

    int a, b, c;
    while (cin >> a >> b) {
        c = a + b;
        printf("%p\n", (void*)&a);
        printf("%p\n", (void*)&b);
        printf("%p\n", (void*)&c);
        printf("\n");
        printf("%d\n", a);
        printf("%d\n", b);
        printf("%d\n", c);
        
        printf("%p\n", (void*)&a);
        printf("%p\n", (void*)&b);
        printf("%p\n", (void*)&c);
        printf("\n");
        printf("%d\n", a);
        printf("%d\n", b);
        printf("%d\n", c);
        
        break;

    }


    return 0;
}




