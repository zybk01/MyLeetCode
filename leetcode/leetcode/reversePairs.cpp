#include "stdafx.h"
#include<queue>
#include<stack>
#include<algorithm>

using namespace std;

class Solution {
public:
	int reversePairs(vector<int>& nums) {
		int r = nums.size();
		if (r == 0)
			return 0;
		vector<int> temp(r);

		int sum = mergeSort(nums, temp, 0, r - 1);

		return sum;
	}

	int mergeSort(vector<int>& nums, vector<int>& temp, int l, int r) {
		if (l >= r)
		{
				return 0;
		}
		int sum = 0;
		int m = (l + r) / 2;
		sum = mergeSort(nums, temp, l, m) + mergeSort(nums, temp, m + 1, r);
		int x = l, y = m+1, i = l;
		while (x <= m&&y <= r)
		{
			if (nums[x] <= nums[y])
			{
				temp[i] = nums[x];
				sum += y - m-1;
				x++;
			}
			else
			{
				temp[i] = nums[y];

				y++;
				
			}
			i++;
		}

		for (; x <= m; x++)
		{
			temp[i] = nums[x];
			i++;
			sum += r - m;
		}
		for (; y <= r; y++)
		{
			temp[i] = nums[y];
			i++;
		}
		for (int k = l; k <=r; k++)
		{
			nums[k] = temp[k];
		}

		return sum;

	}
};




//int main()
//{
//	Solution sol;
//	cout << sol.reversePairs(vector<int>({ 233,2000000001,234,2000000006,235,2000000003,236,2000000007,237,2000000002,2000000005,233,233,233,233,233,2000000004 })) << endl;
//
//
//	cout << "Done!!!" << endl;
//	system("pause");
//	return 0;
//}
