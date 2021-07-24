#include "stdafx.h"
#include<vector>
#include<iostream>
#include<algorithm>

using namespace std;


class Solution {
public:
	int jump(vector<int>& nums) {
		int length = nums.size();
		if (length==1)
		{
			return 0;
		}
		int steps = 1;
		int far = nums[0];
		int pos = far;
		int i = 0;
		while (far<(length-1))
		{
			steps++;
			while (i<=pos)
			{
				far = max(i + nums[i], far);
				i++;
			}
			pos = far;
		}
		return steps;

	}
};