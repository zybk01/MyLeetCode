// #include "stdafx.h"
#include "isBipartite.h"
#include "solutionEntry.h"
#include "zybkLog.h"
#include <queue>
#include <vector>

using namespace std;

bool SolutionIsBipartite::isBipartite(vector<vector<int>> &graph)
{
	int len = graph.size();
	int *map = new int[len]{0};
	queue<int> q;
	for (int i = 0; i < len; i++)
	{
		if (map[i] == 0)
		{
			map[i] = 1;
			q.push(i);
			while (!q.empty())
			{
				int e = q.front();
				q.pop();
				for (int j = 0; j < graph[e].size(); j++)
				{
					if (map[graph[e][j]] == 0)
					{
						map[graph[e][j]] = 3 - map[e];
						q.push(graph[e][j]);
					}
					else if (map[graph[e][j]] == map[e])
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}

extern "C" __declspec(dllexport) void Entry(solutionEntryBase *entry);

void create(pHandle &out)
{
	LOGD("Created!");
	out = reinterpret_cast<pHandle>(new SolutionIsBipartite);
}

void solve(pHandle handle, vector<vector<int>> &input, bool &out)
{
	LOGD("Solving!");
	auto *pSolution = reinterpret_cast<SolutionIsBipartite *>(handle);
	out = pSolution->isBipartite(input);
	return;
}
void destroy(pHandle handle)
{
	LOGD("Destroyed!");
	delete reinterpret_cast<SolutionIsBipartite *>(handle);
}

void Entry(solutionEntryBase *entry)
{
	LOGD("Get Entry!");
	entry->create = create;
	entry->solve = reinterpret_cast<void *>(solve);
	entry->destroy = destroy;
}