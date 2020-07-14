#include<iostream>
using namespace std;

class A{
public:
    int _data[10] = {0};
};

int main(){
    
    // A* a = new A;
    // A* test = new A;
    // size_t* pp;
    // size_t t = 20;
    //pp = &t;
    // for(int i = 0; i < 10; ++i) a->_data[i] = i;
    
    // void* p = a;
    // size_t ptr = (size_t)a;
    // void* tmp = a;
    // size_t* t = (size_t*) tmp;

    // size_t *a = new size_t[1000];
    // int* b = new int[42];
    // for(int i = 0; i < 1000; ++i) a[i] = i;
    // void* p = (void*)a;
    // size_t* ptr = (size_t*)p;
    // // // cout << a[-1] << endl;
    // cout << (*((size_t*)ptr - 1) - sizeof(size_t)) / sizeof(size_t) << endl;
    // cout << *(a-1) << endl;
    // cout << (*((size_t*)(b)-1) - sizeof(size_t)) / sizeof(int) << endl;    

    A* a = new A;


    void* p;
    
    size_t t = (size_t)(void*)a;
    cout << t << endl;

    for(int i = 0; i < 10; i++) a->_data[i] = i;

    p = (void*)t;
    int* ptr = (int*)p;
    cout << *ptr << endl;

    return 0;
}