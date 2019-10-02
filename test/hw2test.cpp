# include <iostream>
# include <cstring>
using namespace std;

int main() {
    char* _readBufPtr;
    char* _readBufEnd;
    char _readBuf[10] = {'\0'};
   
   for(int i = 0; i < 10; i++) {
       _readBuf[i] = 97 + i;
   }
   _readBufPtr = &_readBuf[5];
   cout << (_readBuf <= _readBufPtr) << endl;
   cout << (_readBufPtr <= _readBuf + 10) << endl;
   // cout << (_readBufEnd > _readBufPtr) << endl;
   string s = _readBuf;
   for(int i = 0; i < s.length(); i++) {
       cout << s[i];
   }
   for(int i = 0; i < s.length() - (_readBufPtr - _readBuf); i++) {
       cout << '\b';
   }
   cout << "kkkkk";
   cout << endl;


    return 0;
}