#include "stdafx.h"
#include<vector>
#include<iostream>


using namespace std;


class Solution {
public:
	vector<int> singleNumbers(vector<int>& nums) {
		int a = 0,b=0,c=0;
		int length = nums.size();
		int mark = 0;
		vector<int> d;
		for (int i = 0; i < length; i++)
		{
			a ^= nums[i];
		}
		for (int i = 0; i < sizeof(int)*8; i++)
		{
			if (a&(1 << i)) {
				mark = (1 << i);
			}
		}
		for (int i = 0; i < length; i++)
		{
			if (mark&nums[i]) {
				b ^= nums[i];
			}
			else
			{
				c ^= nums[i];
			}
		}
		d.push_back(b);
		d.push_back(c);

		return d;
	}
};