// #include "stdafx.h"
#include "solutionEntry.h"
#include "zybkLog.h"
#include <algorithm>
#include <queue>
#include <stack>
#include <unordered_map>
#include"restoreArray.h"

using namespace std;

vector<int> SolutionRestoreArray::restoreArray(vector<vector<int>> &adjacentPairs)
{
    int size = adjacentPairs.size();
    vector<int> oriArray(size + 1);
    unordered_map<int, vector<int>> adjacentMap;
    for (vector<int> tmp : adjacentPairs)
    {
        // if(adjacentMap.count(tmp[0])>0){
        //     adjacentMap[tmp[0]].push_back(tmp[1]);
        // }else{
        //     adjacentMap[tmp[0]] = vector<int>({tmp[1]});
        // }
        // if(adjacentMap.count(tmp[1])>0){
        //     adjacentMap[tmp[1]].push_back(tmp[0]);
        // }else{
        //     adjacentMap[tmp[1]] = vector<int>({tmp[0]});
        // }
        adjacentMap[tmp[0]].push_back(tmp[1]);
        adjacentMap[tmp[1]].push_back(tmp[0]);
    }
    // for(map<int,vector<int>>::iterator itr=adjacentMap.begin())
    for (pair<int, vector<int>> tmp : adjacentMap)
    {
        if (tmp.second.size() == 1)
        {
            oriArray[0] = tmp.first;
            break;
        }
    }
    for (int i = 0; i < size; i++)
    {
        vector<int> toFill = adjacentMap[oriArray[i]];
        if (adjacentMap[toFill[0]].size() > 0)
        {
            oriArray[i + 1] = toFill[0];
        }
        else
        {
            oriArray[i + 1] = toFill[1];
        }
        // adjacentMap.erase(oriArray[i]);
        adjacentMap[oriArray[i]].clear();
    }
    return oriArray;
}

extern "C" __declspec(dllexport) void Entry(solutionEntryBase *entry);

void create(pHandle &out)
{
    LOGD("Created!");
    out = reinterpret_cast<pHandle>(new SolutionRestoreArray);
}

void solve(pHandle handle, vector<vector<int>> &input, vector<int> &out)
{
    LOGD("Solving!");
    auto *pSolution = reinterpret_cast<SolutionRestoreArray *>(handle);
    out = pSolution->restoreArray(input);
    return;
}
void destroy(pHandle handle)
{
    LOGD("Destroyed!");
    delete reinterpret_cast<SolutionRestoreArray *>(handle);
}

void Entry(solutionEntryBase *entry)
{
    LOGD("Get Entry!");
    entry->create = create;
    entry->solve = reinterpret_cast<void *>(solve);
    entry->destroy = destroy;
}