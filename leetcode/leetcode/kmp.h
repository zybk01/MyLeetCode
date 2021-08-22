#ifndef _KMP_H_
#define _KMP_H_

#include "solutionEntry.h"
#include "zybkLog.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

using namespace std;

class __declspec(dllexport) SolutionKMP
{
public:
    int kmp(string &T, string &P);
    vector<int> computePrefix(string &P);
};

#endif