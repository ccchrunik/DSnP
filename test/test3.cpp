#include<iostream>
using namespace std;

class A{
public:
    int _data[10];
};

int main(){
    
    A* a = new A;
    A* test = new A;
    // size_t* pp;
    size_t t = 20;
    //pp = &t;
    for(int i = 0; i < 10; ++i) a->_data[i] = i;

    // for(int i = 0; i < 10; ++i) cout << a->_data[i] << endl;
    void* p = test;
    //size_t* ptr = (size_t*)p;
    //*ptr = *test;
    size_t ptr = (size_t)p;
    void *p_a = a;
    size_t *s = (size_t*)p_a;
    *s = ptr; 
    //*ptr = pp;
    for(int i = 0; i < 10; ++i) cout << a->_data[i] << endl;
    cout << p << endl;
    // cout << hex << ptr << endl;
    void *pp = (void*)ptr;
    cout << pp << endl;
    return 0;
}