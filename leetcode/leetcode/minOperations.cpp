#include "stdafx.h"
#include<queue>
#include<vector>
#include<algorithm>
#include<unordered_map>

using namespace std;


class Solution {
public:
    int minOperations(vector<int>& target, vector<int>& arr) {
        vector<vector<int>> dp(target.size(), vector<int>(arr.size()));
        for (int i = 0; i < target.size();i++){
            for (int j = 0; j < arr.size();j++){
                if(target[i]==arr[j]){
                    dp[i][j] = (i>0?dp[i - 1][j]:0)+1;
                }else{
                    dp[i][j] = max(i>0?dp[i - 1][j]:0, j>0?dp[i][j - 1]:0);
                }
            }
        }
        return target.size() - dp[target.size() - 1][arr.size() - 1];
    }
    int minOperations1(vector<int>& target, vector<int>& arr) {
        vector<int> mArr = arr;
        vector<int> dp(arr.size());
        int Max = 0;
        unordered_map<int, int> mMap;

        for (int i = 0; i < target.size();i++){
            mMap[target[i]] = i;
        }
        for (int j = 0; j < arr.size();j++){

            mArr[j] = mMap.count(arr[j]) > 0 ? mMap[arr[j]] : -1;
        }
        dp[0]=mMap.count(arr[0]) > 0 ? 1 : 0;
        for (int j = 0; j < arr.size(); j++){
            for (int k = 0; k < j;k++){
                dp[j] = max(dp[j],((j > 0 ? (mArr[j] > mArr[k] ? dp[k] : 0) : 0) + (mArr[j] >= 0 ? 1 : 0)));
            }
                
            Max = max(Max, dp[j]);
        }
        return target.size()-Max;
    }
};