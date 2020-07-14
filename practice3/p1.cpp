#include <iostream>
#include <string>

using namespace std;

class Base
{
public:
    virtual void f() { cout << "Base::f()" << endl; };
    void g() { cout << "Base::g()" << endl; };
private:
};

class Derived : public Base
{
public:
    void f() { cout << "Derived::f()" << endl; };
    void g() { cout << "Derived::g()" << endl; };
private:
};


int main() {
    Base b; Derived d;
    
    b.f(); b.g();
    d.f(); d.g();

    return 0;
}