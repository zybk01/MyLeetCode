#include "stdafx.h"
#include<queue>
#include<stack>
#include<algorithm>

using namespace std;

//��vivo�����ϣ�ÿλְ�����Ŷ��ֻ��ӹ�������ʶ����Ϥ�;�������ӣ��ղ���Ҳ�᲻��������
//�����һ������1̨��������2��(���ڶ�������)ÿ������2����������3��(�����ġ��塢����)ÿ������3�� ... ...
//�Դ����ƣ����̼������n���ܹ������������ֻ�������

class Solution {
public:
	/**
	*
	* @param n int���� ��n��
	* @return int����
	*/
	int solution(int n) {
		// write code here
		int x = sqrt(2 * n + 0.25) - 0.5;
		int sum = 0;
		for (int i = 1; i <= x; i++) {
			sum += i*i;
		}
		sum += (x + 1)*(n - (1 + x)*x / 2);
		return sum;
	}
};