#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;



int main() {
    // int t = (1 << 31);
  
    int a = 10;
    int arr[10] = { 0 };
    int *arrP[10];

    for (int i = 0; i < 10; ++i) {
        arrP[i] = &arr[i];
        // cout <<&arr[i] << endl;
    }
    // cout << endl;

    int *p1 = new int(10);
    int *p2 = new int[10];
    // cout << &p2 << endl;

    for (int i = 0; i < 10; ++i) {
        // cout << &p2[i] << endl;
    }
    
    int **p3 = new int*;
    *p3 = new int(20);

    int **p4 = new int*[10];
    cout << &p4 << endl;
    for (int i = 0; i < 10; ++i) {
        p4[i] = new int(i + 2);
        cout << &p4[i] << endl;
    }
        
    cout << endl;

    int **p5 = new int*[10];
    cout << &p5 << endl;
    for (int i = 0; i < 10; ++i) {
        p5[i] = new int[i+2];
        cout << &p5[i] << endl;
    }
    
    return 0;
}




