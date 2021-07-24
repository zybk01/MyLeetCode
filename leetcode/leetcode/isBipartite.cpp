#include"stdafx.h"
#include<vector>
#include<queue>

using namespace std;

class Solution {
public:
	bool isBipartite(vector<vector<int>>& graph) {
		int len = graph.size();
		int *map = new int[len] {0};
		queue<int> q;
		for (int i = 0; i<len; i++)
		{
			if (map[i] == 0) {
				map[i] = 1;
				q.push(i);
				while (!q.empty()) {
					int e = q.front();
					q.pop();
						for (int j = 0; j<graph[e].size(); j++) {
							if (map[graph[e][j]] == 0) {
								map[graph[e][j]] = 3 - map[e];
								q.push(graph[e][j]);

							}
							else if (map[graph[e][j]] == map[e]) {
								return false;
							}


						}
					
				}

			}
		}
		return true;
	}
};