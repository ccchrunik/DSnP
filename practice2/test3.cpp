#include <stdio.h>
#include <iostream>

using namespace std;

class T
{
    public:
        T(int a)
        {
            _d = a;
        }
        void f() { _d += 10; };
        const void p() { cout << _d << endl; }
    private:
        int _d = 0;
        
};

int main() {
    const T a(10);
    // Weird!! Below methods are all failed!!!
    // a.f(); Error! passing ‘const T’ as ‘this’ argument discards qualifiers
    // T(a).f().p(); a.p(); Error! passing ‘const T’ as ‘this’ argument discards qualifiers 
    // const_cast<T *>(&a)->f()->p(); a.p(); Error! passing ‘const T’ as ‘this’ argument discards qualifiers 
    return 0;
}