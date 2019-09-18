# include <iostream>
# include <fstream>
# include <string>
# include <vector>

using namespace std;

int main() {
    string inputFile;
    cout << "Please enter the file name: ";
    cin >> inputFile;
    //inputFile = "test1.json";
    //cout << endl << inputFile;
    fstream file;
    
    string line;
    string word = "";
    vector<string> words;
    file.open("test1.json", ios::in);
    // read file line by line
    if(file.is_open()) {
        while ( getline (file, line) ) {
            if(line != "{" || line != "}") {

               for(auto x : line) {
                  if(x == ' ') {
                     words.push_back(word);
                     word = "";
                  } else {
                     word = word + x;
                  }
               }
               string key = words[0];
               int value = stoi(words[1]);
               cout << key << ": " << value << endl;
            }
        }
        file.close();
    }


    return 0;

}