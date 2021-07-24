#include "stdafx.h"
#include<vector>
#include<iostream>
#include<algorithm>

using namespace std;

class Solution {
public:
	int mincostTickets(vector<int>& days, vector<int>& costs) {
		vector<int> dp;
		dp.push_back(0);
		int duration[] = { 1,7,30 };
		int length = days.size();
		int sum = 0x7fffffff;
		for (int i = 0; i < length; i++)
		{
			sum = 0x7fffffff;
			for (int j = 0; j < 3; j++)
			{
				int temp = i;
				
				while (temp >= 0 && days[temp] > days[i] - duration[j]) {
					temp--;
				}
				sum = min(dp[temp+1] + costs[j], sum);
			}
			dp.push_back(sum);
		}
		return dp[length];
	}
};