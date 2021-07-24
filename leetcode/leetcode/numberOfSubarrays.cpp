#include"stdafx.h"
#include<vector>
#include<algorithm>


using namespace std;

class Solution {
public:
	int numberOfSubarrays(vector<int>& nums, int k) {
		int len = nums.size();
		vector<int> vec;
		int count = 0;
		int lastidx = 0;
		for (int i = 0; i < len; i++)
		{
			if (nums[i] % 2 == 1) {
				lastidx = i;
				vec.push_back(i );
				//if (count == 0)
				//{
				//	vec.push_back(i + 1);
				//}
				//else {
				//	vec.push_back(i - vec.back());
				//}
				count++;


			}
		}

		if (count<k)
		{
			return 0;
		}
		len = 0;
		int i = 0;
		
		for (i = 0; i + k < count; i++)
		{
			if (i==0)
			{
				len += (vec[i] + 1)*(vec[i + k] - vec[i + k - 1]);
			}
			else {
				len += (vec[i]-vec[i-1])*(vec[i + k] - vec[i + k - 1]);
			}
			
		}
		if (i==0)
		{
			len += (vec[i]+1 )*(nums.size() - lastidx);
		}
		else {
			len += (vec[i] - vec[i - 1])*(nums.size() - lastidx);
		}
		



		return len;
	}
};



//
//int main()
//{
//	Solution sol;
//	//cout<<sol.getMaxRepetitions("lovelive", 10000, "lovelive", 10000)<<endl;
//	vector<int> vec = { 2,2,2,1,2,2,1,2,2,2 };
//
//
//
//	cout << sol.numberOfSubarrays(vec, 2) << endl;
//	return 0;
//}
