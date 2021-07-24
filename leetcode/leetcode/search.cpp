#include "stdafx.h"
#include<queue>
#include<stack>
#include<algorithm>

using namespace std;

class Solution {
public:
	int search(vector<int>& nums, int target) {
		int length = nums.size();
		int l = 0;
		int r = length-1;
		if (length == 1) {
			if (target==nums[0])
			{
				return 0;
			}
			else {
				return -1;
			}
		}
		while (l<=r)
		{
			if (target>nums[(l+r)/2])
			{
				if (nums[(l + r) / 2]<nums[r])
				{
					if (target==nums[r])
					{
						return r;
					}
					else if (target < nums[r])
					{
						l = (l + r) / 2 + 1;
					}
					else
					{
						r = (l + r) / 2-1 ;
					}
					
				}
				else
				{
					l = (l + r) / 2 + 1;
				}
			}
			else if (target<nums[(l + r) / 2])
			{
				if (nums[(l + r) / 2]>=nums[l])
				{
					if (target == nums[l])
					{
						return l;
					}
					else if (target > nums[l])
					{
						r = (l + r) / 2-1 ;
					}
					else
					{
						l = (l + r) / 2+1;
					}

				}
				else
				{
					r = (l + r) / 2-1 ;
				}
			}
			else
			{
				return (l + r) / 2;
			}
		}

		return -1;
	}
};