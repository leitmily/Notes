#include <iostream>
#include <vector>
#include <string.h>


using namespace std;

vector<int> getNext(const char *str)
{
    int len = strlen(str);
    vector<int> next(len, 0);
    for(int i = 1; i < len; i++)
    {
        int k = next[i - 1];
        while(k > 0 && str[i] != str[k])
            k = next[k - 1];
        if(str[i] == str[k])
            k++;
        next[i] = k;
    }
    return next;
}

int search(const char *str1, const char *str2)
{
    vector<int> next = getNext(str2);
    int k = 0;
    int res = -1;
    for(int i = 0; i < (int)strlen(str1); i++)
    {
        while(k > 0 && str1[i] != str2[k])
            k = next[k - 1];
        if(str1[i] == str2[k])
            k++;
        if(k == (int)strlen(str2))
        {
            res = i - k + 1;
            k = next[k - 1];
        }
    }
    return res;
}

int main()
{
    const char *str1 = "bacbbacabadababacambabacadbacabacasdsd";
    const char *str2 = "bacabaca";
    for(auto i : getNext(str2))
        cout << i << endl;
    cout << "start index is: " << search(str1, str2) << endl;
    return 0;
}