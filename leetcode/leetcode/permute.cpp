#include "stdafx.h"
#include<queue>
#include<stack>
#include<algorithm>

using namespace std;


class Solution {
public:
	vector<vector<int>> permute(vector<int>& nums) {
		vector<int> vec = nums;
		vector<vector<int>> res;
		backtrace(res, nums, 0);
		return res;
	}
	void backtrace(vector<vector<int>> &res,vector<int> nums,int level) {
		int length = nums.size();
		if (level==length)
		{
			res.push_back(nums);
			return;
		}
		int temp = 0;
		for (int i = level; i < length; i++)
		{
			temp = nums[i];
			nums[i] = nums[level];
			nums[level] = temp;
			backtrace(res,nums, level + 1);
			temp = nums[i];
			nums[i] = nums[level];
			nums[level] = temp;
		}
	}
};