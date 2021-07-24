#include "stdafx.h"
#include<queue>
#include<stack>
#include<algorithm>

using namespace std;

//�ָ������������� n����Ѱ�Ҳ������С�������� m��m>9����ʹ�� m �ĸ�λ����λ��ʮλ����λ ... ...��֮�˻�����n��������������� - 1��

class Solution {
public:
	/**
	* ����һ��������ֵ������һ������ֵ
	* @param n int���� n>9
	* @return int����
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