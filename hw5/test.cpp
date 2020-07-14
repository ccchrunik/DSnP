#include <iostream>

using namespace std;

int main()
{
    int N;
    cin >> N;
    char **arr;
    arr = new char*[N];
    for(int i = 0; i < N; ++i)
    {
        arr[i] = new char[50];
        cin >> arr[i];
    }

    return 0;    
}