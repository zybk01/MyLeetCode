#include "stdafx.h"
#include<queue>
#include<stack>
#include<algorithm>

using namespace std;

//现给定任意正整数 n，请寻找并输出最小的正整数 m（m>9），使得 m 的各位（个位、十位、百位 ... ...）之乘积等于n，若不存在则输出 - 1。

class Solution {
public:
	/**
	* 输入一个整形数值，返回一个整形值
	* @param n int整型 n>9
	* @return int整型
	*/
	int solution(int n) {
		// write code here
		vector<int> arr;
		int i = 0;
		int pre = 9;
		while (n >= 10) {
			for (i = pre; i>1; i--) {
				if (n%i == 0) {
					pre = i;
					arr.push_back(i);
					n = n / i;
					break;
				}
			}
			if (i == 1) {
				return -1;
			}

		}
		arr.push_back(n);
		sort(arr.begin(), arr.end());
		int sum = 0;
		int l = arr.size();
		for (int i = 0; i<l; i++) {
			sum = sum * 10 + arr[i];
		}
		return sum;
	}
};