#include <iostream>
#include <string>

using namespace std;


class B
{
public:
    void setptr(int v) { *ptr = v; }
private:
    int* ptr;
};

class A
{
public:
    int convert_int();
    bool convert_bool();
    B convert_B();
private:
    int _data;
};

int A::convert_int()
{
    return _data;
}

bool A::convert_bool()
{
    return (_data != 0);
}

B A::convert_B(){
    B b;
    b.setptr(_data);
    return b;
}


int main() {
    
    return 0;
}