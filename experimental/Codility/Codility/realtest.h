#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <limits>
#include <map>
#pragma once

using namespace std;

bool evenOccurences(string s, char c)
{
    return count(s.begin(), s.end(), c) % 2 == 0;
}

int charactersCopied(map<char, int> occ)
{
    int sum = 0;
    for (auto b : occ)
    {
        sum += b.second;
    }
    return sum;
}

string solution_realtest(int N) {

    string result = "";

    map<char, int> occurences = {};
    while (charactersCopied(occurences) < N)
    {
        for (auto c : "abcdefghijklmnopqrstuvwxyz")
        {
            if ((occurences.find(c) == occurences.end()) || occurences.at(c) % 2 == 0)
            {
                result += c;
                occurences[c]++;
                break;
            }
        }
    }
    return result;
}

