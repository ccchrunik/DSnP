#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

string to_upper(string &input)
{
    string comm = "";
    int a, n;
    for (int i = 0; i < input.length(); i++)
    {
        n = (int)input[i];
        if (input[i] != ' ' && (n < 48 || n > 57))
        {
            a = (int)input[i] - 32;
            comm += (char)a;
        }
        else
        {
            comm += input[i];
        }
    }
    return comm;
}

vector<string> to_parse(string &line)
{
    string word = "";
    vector<string> words;
    for (int i = 0; i < line.length(); i++)
    {
        // cout << line[i] << " ";
        if (line[i] != ' ')
        {
            word += line[i];
        }
        else
        {
            words.push_back(word);
            word = "";
        }
    }
    if (word.length() > 0)
    {
        words.push_back(word);
    }

    return words;
}

int main()
{
    while (true)
    {
        string input;
        string comm;
        vector<string> argv;
        getline(cin, input);
        comm = to_upper(input);
        cout << comm << endl;
        argv = to_parse(comm);
        cout << argv.size() << endl;
        cout << argv[0] << " " << argv[1] << " " << argv[2] << endl;
    }

    return 0;
}