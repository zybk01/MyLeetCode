#include "stdafx.h"
#include <algorithm>
#include <queue>
#include <vector>

using namespace std;

class Solution
{
public:
	int numIslands(vector<vector<char>> &grid)
	{
		int m = grid.size();
		if (m == 0)
		{
			return 0;
		}
		int n = grid[0].size();
		queue<pair<int, int>> que;
		int result = 0;

		int dx[] = {1, 0, -1, 0};
		int dy[] = {0, 1, 0, -1};
		for (int i = 0; i < m; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if (grid[i][j] == '1')
				{
					result++;
					grid[i][j] = '0';
					que.push({i, j});
					while (!que.empty())
					{
						int xx = que.front().first;
						int yy = que.front().second;
						que.pop();
						for (int k = 0; k < 4; k++)
						{
							int x = xx + dx[k];
							int y = yy + dy[k];
							if (x >= 0 && x < m && y >= 0 && y < n)
							{
								if (grid[x][y] == '1')
								{
									grid[x][y] = '0';
									que.push({x, y});
								}
							}
						}
					}
				}
			}
		}
		return result;
	}
};