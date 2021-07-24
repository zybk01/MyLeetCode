#include "stdafx.h"
#include<queue>
#include<stack>
#include<algorithm>

using namespace std;

//现有一个 3x3 规格的 Android 智能手机锁屏程序和两个正整数 m 和 n ，请计算出使用最少m 个键和最多 n个键可以解锁该屏幕的所有有效模式总数。
//其中有效模式是指：
//1、每个模式必须连接至少m个键和最多n个键；
//2、所有的键都必须是不同的；
//3、如果在模式中连接两个连续键的行通过任何其他键，则其他键必须在模式中选择，不允许跳过非选择键（如图）；
//4、顺序相关，单键有效（这里可能跟部分手机不同）。
//
//输入：m, n
//代表允许解锁的最少m个键和最多n个键
//输出：
//满足m和n个键数的所有有效模式的总数

//例子说明1:
//输入m = 1，n = 2，表示最少1个键，最多2个键，符合要求的键数是1个键和2个键，其中1个键的有效模式有9种，两个键的有效模式有56种，所以最终有效模式总数是9 + 56 = 65种，最终输出65。

class Solution {
public:
	/**
	* 实现方案
	* @param m int整型 最少m个键
	* @param n int整型 最多n个键
	* @return int整型
	*/
	int sum = 0;
	int nums[10] = { 0,1,2,3,4,5,6,7,8,9 };
	int solution(int m, int n) {
		// write code here
		if (n>9)
		{
			n = 9;
		}
		solutionmove(nums, 0, 1, m, n);
		return sum;
	}
	void solutionmove(int nums[], int pre, int level, int m, int n) {

		for (int i = level; i <= 9; i++) {
			if (pre>0 && checkoverlap(pre, nums[i], level, nums)) {
				continue;
			}
			else {
				if (level >= m&&level <= n) {
					sum++;
				}
				int temp = nums[i];
				nums[i] = nums[level];
				nums[level] = temp;
				solutionmove(nums, temp, level + 1, m, n);
				nums[level] = nums[i];
				nums[i] = temp;
			}
		}
	}
	bool checkoverlap(int m, int n, int level, int nums[]) {
		if (abs((m - 1) / 3 - (n - 1) / 3) == 2 && (m - 1) % 3 == (n - 1) % 3) {
			for (int i = level; i <= 9; i++) {
				if ((4 + (n - 1) % 3) == nums[i]) {
					return true;
				}
			}
		}
		else if (abs((m - 1) % 3 - (n - 1) % 3) == 2 && (m - 1) / 3 == (n - 1) / 3) {
			for (int i = level; i <= 9; i++) {
				if ((2 + (n - 1) / 3 * 3) == nums[i]) {
					return true;
				}
			}
		}
		else if (abs((m - 1) % 3 - (n - 1) % 3) == 2 && abs((m - 1) / 3 - (n - 1) / 3) == 2) {
			for (int i = level; i <= 9; i++) {
				if (5 == nums[i]) {
					return true;
				}
			}
		}
		return false;
	}
};
