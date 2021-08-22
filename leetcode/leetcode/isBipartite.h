#ifndef _ISPARITIE_H_
#define _ISPARTITE_H_

// #include "stdafx.h"
#include <queue>
#include <vector>
#include"zybkLog.h"
#include"solutionEntry.h"


using namespace std;

class __declspec(dllexport) SolutionIsBipartite
{
public:
    bool isBipartite(vector<vector<int>> &graph);
};


#endif