#ifndef _RESTOREARRAY_H_
#define _RESTOREARRAY_H_

#include "solutionEntry.h"
#include "zybkLog.h"
#include <algorithm>
#include <queue>
#include <stack>
#include <unordered_map>

using namespace std;

class __declspec(dllexport) SolutionRestoreArray
{
public:
    vector<int> restoreArray(vector<vector<int>> &adjacentPairs);
};


#endif