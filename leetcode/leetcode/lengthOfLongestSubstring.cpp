#include "stdafx.h"
#include<vector>
#include<iostream>


using namespace std;

class Solution {
public:
	int lengthOfLongestSubstring(string s) {
		int longest = 1;
		int length = s.length();
		if (length == 0) {
			return 0;
		}
		int *dp = new int[length]();
		dp[0] = 1;
		for (int i = 1; i < length; i++)
		{
			dp[i] = 1;
			for (int j = 1; j <= dp[i-1]; j++)
			{
				if (s[i]!=s[i-j])
				{
					dp[i]++;
				}
				else
				{
					break;
				}
			}
			if (dp[i] > longest) {
				longest = dp[i];
			}

		}

		return longest;
	}
};