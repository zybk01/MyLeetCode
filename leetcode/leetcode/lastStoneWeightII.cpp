#include "stdafx.h"
#include<vector>
#include<iostream>
using namespace std; 

//int main()
//{
//	Solution sol;
//	vector<int> vec;
//	vec.push_back(8);
//	vec.push_back(2);
//	vec.push_back(4);
//	vec.push_back(4);
//	vec.push_back(8);
//
//
//	cout << sol.lastStoneWeightII(vec) << endl;
//	return 0;
//}


class Solution {
public:
	int lastStoneWeightII(vector<int>& stones) {
		int sum = 0;
		int l = stones.size();
		for (int i=0;i<l;i++)
		{
			sum += stones[i];
		}
		int *dp = new int[sum / 2 + 1]{0};
		dp[0] = 1;
		for (int i = 0; i < l; i++)
		{
			for (int j = sum / 2; j >=stones[i]; j--) 
			{
				dp[j] = dp[j] | dp[j - stones[i]];
			}
		}
		for (int i = sum / 2; i >0; i--)
		{
			cout<<dp[i] << endl;
		}
		for (int i = sum/2; i >0; i--)
		{
			if (dp[i]>0) {
				return sum - 2 * i;
			}
		}
		return 0;


	}
};