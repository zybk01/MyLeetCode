#ifndef _PERMUTE_H_
#define _PERMUTE_H_

#include "solutionEntry.h"
#include "zybkLog.h"
#include <algorithm>
#include <queue>
#include <stack>

using namespace std;

class __declspec(dllexport) SolutionPermute
{
public:
    vector<vector<int>> permute(vector<int> &nums);
    void backtrace(vector<vector<int>> &res, vector<int> nums, int level);
};

#endif