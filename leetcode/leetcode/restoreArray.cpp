#include "stdafx.h"
#include<queue>
#include<stack>
#include<algorithm>
#include<unordered_map>

using namespace std;


class Solution {
public:
	vector<int> restoreArray(vector<vector<int>>& adjacentPairs) {
        int size = adjacentPairs.size();
        vector<int> oriArray(size+1);
        unordered_map<int, vector<int>> adjacentMap;
        for(vector<int> tmp:adjacentPairs){
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
        for(pair<int,vector<int>> tmp:adjacentMap){
            if(tmp.second.size()==1){
                oriArray[0] = tmp.first;
                break;
            }
        }
        for (int i = 0; i < size; i++){
            vector<int> toFill = adjacentMap[oriArray[i]];
            if(adjacentMap[toFill[0]].size()>0){
                oriArray[i + 1] = toFill[0];
            }else{
                oriArray[i + 1] = toFill[1];
            }
            // adjacentMap.erase(oriArray[i]);
            adjacentMap[oriArray[i]].clear();
        }
        return oriArray;
	}
};