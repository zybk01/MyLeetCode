#include"stdafx.h"
#include<vector>

using namespace std;



class Solution {
public:
	bool canJump(vector<int>& nums) {
		int l = nums.size();
		if (l==0)
		{
			return true;
		}
		bool *dp = new bool[l] {false};
		dp[0] = true;

		for (int i = 0; i < l; i++)
		{
			for (int j = 0; j < i; j++) {
				if (dp[i] || (dp[j] && (nums[j] + j >= i)))
				{
					dp[i] = true;
				}
			}
			if (dp[i] == false) {
				return false;
			}
		}
		return dp[l - 1];
	}
};