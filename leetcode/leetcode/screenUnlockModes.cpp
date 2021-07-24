#include "stdafx.h"
#include<queue>
#include<stack>
#include<algorithm>

using namespace std;

//����һ�� 3x3 ���� Android �����ֻ�������������������� m �� n ��������ʹ������m ��������� n�������Խ�������Ļ��������Чģʽ������
//������Чģʽ��ָ��
//1��ÿ��ģʽ������������m���������n������
//2�����еļ��������ǲ�ͬ�ģ�
//3�������ģʽ��������������������ͨ���κ�����������������������ģʽ��ѡ�񣬲�����������ѡ�������ͼ����
//4��˳����أ�������Ч��������ܸ������ֻ���ͬ����
//
//���룺m, n
//�����������������m���������n����
//�����
//����m��n��������������Чģʽ������

//����˵��1:
//����m = 1��n = 2����ʾ����1���������2����������Ҫ��ļ�����1������2����������1��������Чģʽ��9�֣�����������Чģʽ��56�֣�����������Чģʽ������9 + 56 = 65�֣��������65��

class Solution {
public:
	/**
	* ʵ�ַ���
	* @param m int���� ����m����
	* @param n int���� ���n����
	* @return int����
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
