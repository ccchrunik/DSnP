#include <iostream>
#include <string>

using namespace std;

class A
{
public:
    A(): _d(0) {}
    A(int v): _d(v) {}
    int getd() const { return _d; } ;
    void setd(int v) { _d = v; };

    A& operator = (const A&);
    A operator + (const A&);
    A operator - (const A&);
    A operator * (const A&);
    A operator / (const A&);
    A& operator+= (int);
    A& operator-= (int);
    A& operator*= (int);
    A& operator/= (int);
    A& operator ++ ();
    A operator ++ (int);
    A& operator --();
    A operator --(int);
    bool operator == (const A&);
    bool operator != (const A&);

    friend ostream& operator << (ostream&, const A&);
    friend istream& operator >> (istream&, const A&);

private:
    int _d;
};

A&
A::operator=(const A& a) {
    return *this;
}

A
A::operator+(const A& a) {
    return A(getd() + a.getd());
}

A
A::operator-(const A& a) {
    return A(getd() - a.getd());
}

A
A::operator*(const A& a) {
    return A(getd() * a.getd());
}

A
A::operator/(const A& a) {
    return A(int(getd() / a.getd()));
}

A&
A::operator+=(int v) {
    _d += v;
    return *this;
}

A&
A::operator-=(int v) {
    _d -= v;
    return *this;
}

A&
A::operator*=(int v) {
    _d *= v;
    return *this;
}

A&
A::operator/=(int v) {
    _d /= v;
    return *this;
}

A&
A::operator++() {
    _d++;
    return *this;
}

A
A::operator++(int) {
    A tmp(_d);
    _d++;
    return tmp;
}

A&
A::operator--() {
    _d--;
    return *this;
}

A
A::operator--(int) {
    A tmp(_d);
    _d--;
    return tmp;
}


bool
A::operator==(const A& a) {
    return getd() == a.getd();
}

bool
A::operator!=(const A& a) {
    return getd() != a.getd();
}

ostream&
operator << (ostream& os, const A& a) {
    os << a._d;
}

istream&
operator >> (istream& is, const A& a) {
    is >> a._d;
}

int main() {
    
    return 0;
}