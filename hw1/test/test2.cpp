#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int string_to_int(string &snum) {
    bool neg = false;
    int num = 0;
    int power = 1;
    if(snum[0] == '-') // negative number
    {
        for(int i = snum.length() - 1; i >= 1; i--)
        {
            num += ((int)snum[i] - 48) * power;
            power *= 10;
        }
        num = -num;
    }
    else // positive number
    {
        for(int i = snum.length() - 1; i >= 0; i--) {
            num += ((int)snum[i] - 48) * power;
            power *= 10;
        }
    }
    
    return num;
}

int main()
{
    string inputFile;
    cout << "Please enter the file name: ";
    cin >> inputFile;

    fstream file;
    string line;
    string word = "";
    int key_flag = 0;
    int num_flag = 0;
    vector<string> words;

    file.open(inputFile, ios::in);
    // read file line by line
    if (file.is_open())
    {
        while (getline(file, line))
        {
            for (int i = 0; i < line.length(); i++)
            {
                int a = int(line[i]);
                if (line[i] == '{' || line[i] == '}')
                {
                    break;
                }
                else if (line[i] == '\"' && key_flag == 0)
                {
                    word += line[i];
                    key_flag = 1;
                }
                else if (line[i] != '\"' && key_flag == 1)
                {
                    word += line[i];
                }
                else if (line[i] == '\"' && key_flag == 1)
                {
                    word += line[i];
                    // cout << word << endl;
                    words.push_back(word);
                    word = "";
                    key_flag = 0;
                }
                else if (a == 45 && num_flag == 0) // -(minus) symbol 
                {
                    word += line[i];
                }
                else if (a >= 48 && a <= 57 && num_flag == 0)
                { // ascii for 0~9
                    num_flag = 1;
                    word += line[i];
                }
                else if (a >= 48 && a <= 57 && num_flag == 1)
                { // keep recording the number(value)
                    word += line[i];
                }
                else if ((a < 48 || a > 57) && num_flag == 1) // stop counting the number
                { // not in 0~9
                    // cout << word << endl;
                    words.push_back(word);
                    word = "";
                    num_flag = 0;
                }
            }

        }

        file.close();
    }
    if (word.length() != 0) // the last element of the json file
    {
        // cout << word << endl;
        words.push_back(word);
        // string key = words[0];
    }
    // for checking: print all the elements
    // cout << words.size() << endl;
    if (words.size() > 0)
    {
        for (int i = 0; i < words.size() - 1; ++i)
        {
            cout << words[i] << " ";
        }
        cout << words[words.size() - 1] << endl;
        cout << string_to_int(words[1]) << endl;

    }

    return 0;
}