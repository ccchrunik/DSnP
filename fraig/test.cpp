#include <iostream>
#include <vector>
using namespace std;

typedef pair<int, vector<int>& > InT;

int main()
{
    // unsigned long sum;
    // int a = INT32_MAX - 1;
    // // sum = a >> 1;
    // // sum = 10000000UL << 30;
    // sum = (unsigned long)INT32_MAX << 40;
    // // sum = (size_t)1 << 40;
    // cout << INT32_MAX << endl;
    // cout << sum << endl;
    // vector<int> num;
    // num.push_back(1);
    // InT p(10, num);
    // p.second.push_back(2);
    // cout << num.size() << endl;

    // for(int i = 0; i < num.size(); ++i)
    // {
    //     cout << num[i] << endl;
    // }
    int num = 0;
    cout << "number: " << num;
    for(int i = 1; i < 100000000; i++)
    {
        int w = 1;
        while(num / 10)
        {
            ++w; num /= 10;
        }
        for(int j = 0; j < w; ++j) cout << '\b';
        for(int j = 0; j < w; ++j) cout << ' ';
        for(int j = 0; j < w; ++j) cout << '\b';

        cout << i;
        num = i;
    }
    cout << endl;


    return 0;
}