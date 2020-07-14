#include <iostream>
#include <string>

using namespace std;

class Base
{
public:
    virtual void f() { cout << "Base::f()" << endl; };
    void g() { cout << "Base::g()" << endl; };
    virtual void h() { cout << "Base::h()" << endl; };
private:
};

class Derived : public Base
{
public:
    void f() { cout << "Derived::f()" << endl; };
    void g() { cout << "Derived::g()" << endl; };
    // void h() { cout << "Derived::h()" << endl; };
private:
};


int main() {
    Base *p = new Derived;
    Base *q = new Base;
    Derived *r = new Derived;
    
    p->f(); p->g(); p->h();
    q->f(); q->g(); q->h();
    r->h(); r->g(); r->h();

    return 0;
}