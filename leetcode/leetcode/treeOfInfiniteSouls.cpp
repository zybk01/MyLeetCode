// #include"stdafx.h"
#include "treeOfInfiniteSouls.h"
#include "zybkLog.h"
#include <algorithm>
#include <cmath>
#include <vector>

using namespace std;

int SolutiontreeOfInfiniteSouls::treeOfInfiniteSouls(vector<int> &gem, int p, int target)
{
    mP = p;
    mTarget = target;
    int num = 0;
    vector<int> GemInfo(gem.size(), 0);
    vector<int> outGem;
    recursiveGem(gem, 0, num, GemInfo, outGem);
    // int finished = 1;
    // vector<int> iterrateCnt;
    // iterrateCnt.resize(gem.size(), 0);
    // int incremented = 0;
    // do
    // {
    //     incremented = 0;
    //     int newIdx = 0;
    //     int lastIdx = 0;
    //     vector<int> newgem(gem.size());
    //     for (int i = 0; i < gem.size(); i++)
    //     {
    //         newgem[i] = gem[iterrateCnt[i]];
    //         if (incremented == 0 && (iterrateCnt[i] < gem.size() - 1 - i))
    //         {
    //             iterrateCnt[i]++;
    //             incremented = 1;
    //             lastIdx = i;
    //         }
    //         if (incremented == 1 && iterrateCnt[i] == 0)
    //         {
    //             if (iterrateCnt[lastIdx] == newIdx + 1)
    //                 newIdx++;
    //             newgem[i] = gem[newIdx];
    //             newIdx++;
    //         }
    //         LOGD("newgem %d : %d", i, newgem[i]);
    //     }
    //     vector<int> resThisTree;
    //     resThisTree.push_back(0);
    //     treeOfInfiniteSouls(newgem, 0, newgem.size(), resThisTree);
    //     for (auto res : resThisTree)
    //     {
    //         LOGD("res %lld", res);
    //         if (res % p == target)
    //         {
    //             num++;
    //         }
    //     }
    // } while (incremented);
    return num;
}

int SolutiontreeOfInfiniteSouls::treeOfInfiniteSouls(vector<int> &gem, int idxS, int idxE,
                                                     vector<int> &result)
{
    vector<int> resThisTree;
    vector<int> resThisLoop;
    for (auto &inNum : result)
    {
        inNum = (Numcompound(inNum, 1));
        if (idxS + 1 == idxE)
        {
            inNum = Numcompound(inNum, gem[idxS]);
            resThisLoop.push_back(inNum);
        }
    }

    for (int i = idxS + 1; i < idxE; i++)
    {
        vector<int> res;
        res = result;
        treeOfInfiniteSouls(gem, idxS, i, res);
        treeOfInfiniteSouls(gem, i, idxE, res);
        resThisLoop.insert(resThisLoop.end(), res.begin(), res.end());
    }
    for (auto &resNum : resThisLoop)
    {
        // for (auto inNum : result)
        // {
        //     long long thisRee = Numcompound(inNum, resNum);
        //     thisRee = Numcompound(thisRee, 9);
        //     // inNum = inNum % mP;
        //     resThisTree.push_back(thisRee);
        //     // LOGD("inNum %lld", inNum);
        // }
        resNum = Numcompound(resNum, 9);
    }
    result = resThisLoop;
    return 0;
}

int SolutiontreeOfInfiniteSouls::Numcompound(int ori, int num)
{
    int temp = num;
    ori = ori % mP;
    num = num % mP;
    int compountd = (ori * 10);
    compountd = compountd % mP;
    while (temp > 10)
    {
        temp = temp / 10;
        compountd = (compountd * 10);
        compountd = compountd % mP;
    }
    compountd = compountd + num;
    return compountd;
}

void SolutiontreeOfInfiniteSouls::recursiveGem(vector<int> &gem, int idx, int &num, vector<int> &GemInfo, vector<int> &outGem)
{
    if (idx == gem.size())
    {
        vector<int> resThisTree;
        resThisTree.push_back(0);
        for (int i = 0; i < gem.size(); i++)
        {
            LOGD("gem %d", outGem[i]);
        }
        treeOfInfiniteSouls(outGem, 0, outGem.size(), resThisTree);
        for (auto res : resThisTree)
        {
            LOGD("res %lld", res);
            if (res % mP == mTarget)
            {
                num++;
            }
        }
    }
    for (int i = 0; i < gem.size(); i++)
    {
        if (GemInfo[i] == 0)
        {
            outGem.push_back(gem[i]);
            GemInfo[i] = 1;
            recursiveGem(gem, idx + 1, num, GemInfo, outGem);
            GemInfo[i] = 0;
            outGem.pop_back();
        }
    }
}

// SolutiontreeOfInfiniteSouls sloutu;
// vector<int> gem({2});
// LOGD("treeOfInfiniteSouls %d ", sloutu.treeOfInfiniteSouls(gem, 100000007, 11391299));