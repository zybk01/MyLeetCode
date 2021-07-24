#include "stdafx.h"
#include<queue>
#include<stack>
#include<algorithm>

using namespace std;


class Solution {
public:
	int waysToChange(int n) {
		int coins[] = { 1,5,10,25 };

		return Dp_rewind(3, n, coins);
	}

	/*improved according to leetcode£¬with better understanding*/
	int Dp_rewind(int var, int n, int coins[4]) {
		int *dp = new int[n+1]();
		dp[0] = 1;
		for (int i = 0; i < 4; i++)
		{
			for (int j = coins[i]; j <= n; j++)
			{
				dp[j] = (dp[j] + dp[j - coins[i]])% 1000000007;
			}
		}

			return dp[n];
	}


};

//int main()
//{
//	Solution sol;
//	int input = 0;
//	for (int i = 0; i < 10; i++) {
//		cout << i << ":";
//		cin >> input;
//		cout << sol.waysToChange(input) << endl;
//	}
//
//	cout << "Done!!!" << endl;
//	system("pause");
//	return 0;
//}

	/*improved according to leetcode*/
	//int Dp_rewind(int var, int n, int coins[4]) {
	//	int sum = 0;
	//	if (var == 0)
	//	{
	//		return 1;
	//	}
	//	if (coins[var] <= n)
	//	{
	//		sum += Dp_rewind(var, n - coins[var], coins);
	//	}
	//	sum += Dp_rewind(var - 1, n, coins);

	//	return sum;
	//}


	/*int Dp_rewind(int var,int n,int coins[4]) {
		int length = n / coins[var];
		int sum = 0;
		if (var==0)
		{
			return 1;
		}
		for (int i = 0; i <= length; i++)
		{
			sum += Dp_rewind(var-1,n-i*coins[var],coins);
		}
		return sum;
	}*/













	/*typical bfs method*/

	/*int waysToChange(int n) {
		int coins[] = { 1,5,10,25 };
		int ways = 0;
		queue<pair<int,int>> que;
		que.push({ 0,0 });
		while (!que.empty())
		{
			int length = que.size();
			int sum = 0;
			int incre = 0;
			int temp = 0;
			for (int i = 0; i < length; i++)
			{
				sum = que.front().first;
				incre = que.front().second;
				que.pop();
				if (sum == n)
				{
					ways++;
					continue;
				}
				temp = n - sum;

				for each (int var in coins)
				{

					if (temp >= var&&var>=incre)
					{
						que.push({ sum + var,var });
					}
				}
			}
		}
		return ways;
	}*/
