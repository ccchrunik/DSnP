# include <iostream>
# include <cstring>
using namespace std;

int main() {
    char* _readBufPtr;
    char* _readBufEnd;
    char _readBuf[10] = {'\0'};

    _readBufPtr = _readBufEnd = _readBuf;

    for(int i = 0; i < 10; i++) {
        _readBuf[i] = 97 + i;
        cout << _readBuf[i];
    }
    cout << "\b\b\b\b\b";
 
    _readBufPtr = &_readBuf[5];

    char* tmp = _readBufPtr;
    string s = tmp;
    char* tmp2 = _readBufPtr;
    int tmp_size = 0;
    while(*tmp2 != '\0') {
        tmp_size++;
        tmp2++;
    }
    cout << endl;
    cout << tmp << endl;
    cout << s << endl;
    _readBuf[9] = 'Z';
    cout << tmp << endl;
    cout << s << endl;
    



    return 0;
}