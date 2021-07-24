// leetcode.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<vector>

using namespace std;

class Solution {
public:
	bool isToeplitzMatrix(vector<vector<int>>& matrix) {
		int m = matrix.size();
		int n = 0;
		if (m>0)
		{
			n = matrix[0].size();
		}
		else {
			return true;
		}
		if (m<n)
		{
			for (int i = 0; i < m; i++) {
				if (matrix[i][i]!=matrix[0][0])
				{
					return false;
				}
			}
		}
		return true;
	}
};