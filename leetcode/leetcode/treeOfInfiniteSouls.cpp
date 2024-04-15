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
    treeOfInfiniteSouls(gem, 0, gem.size(), mResThisTree);
    recursiveGem(gem, 0, num, GemInfo, outGem);
    return num;
}

int SolutiontreeOfInfiniteSouls::treeOfInfiniteSouls(vector<int> &gem, int idxS, int idxE,
                                                     vector<string> &result)
{
    vector<string> resThisTree;
    vector<string> resThisLoop;
    if (result.size() == 0)
    {
        result.push_back("1");
        if (idxS + 1 == idxE)
        {
            // inNum = Numcompound(inNum, gem[idxS]);
            // result[0] = result[0] + to_string(gem[idxS]);
            result[0] = result[0] + "?";
            resThisLoop.push_back(result[0]);
        }
    }
    else
    {
        for (auto &inNum : result)
        {
            // inNum = (Numcompound(inNum, 1));
            inNum = inNum + "1";
            if (idxS + 1 == idxE)
            {
                // inNum = Numcompound(inNum, gem[idxS]);
                // inNum = inNum + to_string(gem[idxS]);
                inNum = inNum + "?";
                resThisLoop.push_back(inNum);
            }
        }
    }

    for (long long i = idxS + 1; i < idxE; i++)
    {
        vector<string> res;
        vector<string> trans;
        resThisTree.clear();
        // res.push_back("0");
        treeOfInfiniteSouls(gem, idxS, i, res);
        for (auto &inNum : result)
        {
            for (auto resNum : res)
            {
                // long long thisRee = Numcompound(inNum, resNum);
                string thisRee = inNum + resNum;
                // LOGD("inNum %lld", thisRee);
                resThisTree.push_back(thisRee);
            }
        }
        trans = resThisTree;
        resThisTree.clear();
        res.clear();
        // res.push_back("0");
        treeOfInfiniteSouls(gem, i, idxE, res);
        for (auto &inNum : trans)
        {
            for (auto resNum : res)
            {
                // long long thisRee = Numcompound(inNum, resNum);
                // LOGD("outNum %lld", thisRee);

                string thisRee = inNum + resNum;
                resThisTree.push_back(thisRee);
            }
        }
        resThisLoop.insert(resThisLoop.end(), resThisTree.begin(), resThisTree.end());
    }
    for (auto &resNum : resThisLoop)
    {
        // resNum = Numcompound(resNum, 9);
        resNum = resNum + "9";
    }
    result = resThisLoop;
    return 0;
}

long long SolutiontreeOfInfiniteSouls::Numcompound(long long ori, long long num)
{
    long long temp = num;
    ori = ori % mP;
    num = num % mP;
    long long compountd = (ori * 10);
    compountd = compountd % mP;
    while (temp >= 10)
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
        vector<string> resThisTree;
        // resThisTree.push_back(0);
        // resThisTree.push_back("0");
        // treeOfInfiniteSouls(outGem, 0, outGem.size(), resThisTree);
        int treeNum = 0;
        for (auto res : mResThisTree)
        {
            LOGD("%s", res.c_str());
            long long resInt = 0;
            int j = 0;
            int i = 0;
            if (cacheMap.begin() != cacheMap.end())
            // if (cacheMap.begin() != cacheMap.end())
            {
                auto treeItr = cacheMap.rbegin();
                if (treeItr->second.count(treeNum))
                {
                    resInt = treeItr->second[treeNum];
                    j = treeItr->first;
                    // i = treeItr->second[treeNum];
                    i = cacheIdxMap[j][treeNum];
                }
            }
            for (; i < res.size(); i++)
            {
                if (res[i] == '?')
                {
                    cacheMap[j][treeNum] = resInt;
                    cacheIdxMap[j][treeNum] = i;
                    resInt = Numcompound(resInt, outGem[j]);
                    // resInt = (resInt *10 + gem[j]) % mP;
                    j++;
                    LOGD("%c %d treeNum %d", res[i], j, treeNum);
                }
                else
                {
                    resInt = (resInt * 10 + res[i] - '0') % mP;
                }
            }
            if (resInt % mP == mTarget)
            {
                num++;
            }
            treeNum++;
        }
    }
    for (int i = 0; i < gem.size(); i++)
    {
        if (GemInfo[i] == 0)
        {
            outGem.push_back(gem[i]);
            GemInfo[i] = 1;
            recursiveGem(gem, idx + 1, num, GemInfo, outGem);
            cacheMap.erase(idx);
            cacheIdxMap.erase(idx);
            GemInfo[i] = 0;
            outGem.pop_back();
        }
    }
}

// SolutiontreeOfInfiniteSouls sloutu;
// vector<int> gem({2});
// LOGD("treeOfInfiniteSouls %d ", sloutu.treeOfInfiniteSouls(gem, 100000007, 11391299));