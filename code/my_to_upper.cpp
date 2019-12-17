#include <iostream>
#include <string>
using namespace std;

string myToUpper(const string str)
{
    string newStr = str;
    for (size_t pos = 0; pos < str.length(); pos++)
    {
        if (str[pos] >= 'a' && str[pos] <= 'z')
        {
            newStr[pos] += ('A' - 'a');
        }
    }
    return newStr;
}

int main()
{
    string orgStr = "jiangjie";
    string newStr = myToUpper(orgStr);
    cout << newStr << endl;
    return 0;
}