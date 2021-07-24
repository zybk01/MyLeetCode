#include"stdafx.h"
#include<vector>
#include<algorithm>


using namespace std;

class Solution {
public:
	int maxArea(vector<int>& height) {
		int len = height.size();
		int l = 0;
		int r = len - 1;
		int result =0;
		for (int i = 0; i<len; i++)
		{
			result= max(min(height[l], height[r])*(r-l),result);
			if (height[l]>height[r])
			{
				r--;
			}
			else {

				l++;
			}
			if (l==r)
			{
				break;
			}
		}
		return result;
	}
}; 