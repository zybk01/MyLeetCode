// #include"stdafx.h"
#include <algorithm>
#include <unordered_map>
#include <vector>

using namespace std;

struct hashFunc
{
    size_t operator()(const pair<int, int>& in) const
    {
        return hash<int>()(in.first) ^ hash<int>()(in.second);
    }
};
class SolutiontreeOfInfiniteSouls
{
public:
    int treeOfInfiniteSouls(vector<int> &gem, int p, int target);
    int treeOfInfiniteSouls(vector<int> &gem, int idxS, int idxE,
                            vector<int> &result);
    int Numcompound(int ori, int num);
    void recursiveGem(vector<int> &gem, int idx, int &num, vector<int> &GemInfo, vector<int> &outGem);
    int mP;

    int mTarget;
    unordered_map<int, vector<int>> mCacheMap;
};