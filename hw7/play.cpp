# include <iostream>
# include <vector>

using namespace std;

int main()
{
    vector<int> t;
    t.reserve(100);
    t.push_back(1);
    cout << (t[1] == 0) << " " << (t[2] == 0) << endl;
    return 0;
}