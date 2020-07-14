#include <iostream>
#include <string>

using namespace std;

class Base
{
public:
    virtual void f() { cout << "Base::f()" << endl; };
    void g() { cout << "Base::g()" << endl; };
    virtual void h() { cout << "Base::h()" << endl; } // = 0;
private:
};

class Derived : public Base
{
public:
    void f() { cout << "Derived::f()" << endl; };
    void g() { cout << "Derived::g()" << endl; };
    // void h();
    // void h() { cout << "Derived::h()" << endl; };
private:
};


int main() {
    Base *p = new Derived;
    Base *q = new Base;
    
    p->f(); p->g(); p->h();
    q->f(); q->g(); q->h();
    

    return 0;
}