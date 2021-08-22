// #include "stdafx.h"
#include "permute.h"
#include "solutionEntry.h"
#include "zybkLog.h"
#include <algorithm>
#include <queue>
#include <stack>

using namespace std;

vector<vector<int>> SolutionPermute::permute(vector<int> &nums)
{
	vector<int> vec = nums;
	vector<vector<int>> res;
	backtrace(res, nums, 0);
	return res;
}
void SolutionPermute::backtrace(vector<vector<int>> &res, vector<int> nums, int level)
{
	int length = nums.size();
	if (level == length)
	{
		res.push_back(nums);
		return;
	}
	int temp = 0;
	for (int i = level; i < length; i++)
	{
		temp = nums[i];
		nums[i] = nums[level];
		nums[level] = temp;
		backtrace(res, nums, level + 1);
		temp = nums[i];
		nums[i] = nums[level];
		nums[level] = temp;
	}
}

extern "C" __declspec(dllexport) void Entry(solutionEntryBase *entry);

void create(pHandle &out)
{
	LOGD("Created!");
	out = reinterpret_cast<pHandle>(new SolutionPermute);
}

void solve(pHandle handle, vector<int> &input, vector<vector<int>> &out)
{
	LOGD("Solving!");
	auto *pSolution = reinterpret_cast<SolutionPermute *>(handle);
	out = pSolution->permute(input);
	return;
}
void destroy(pHandle handle)
{
	LOGD("Destroyed!");
	delete reinterpret_cast<SolutionPermute *>(handle);
}

void Entry(solutionEntryBase *entry)
{
	LOGD("Get Entry!");
	entry->create = create;
	entry->solve = reinterpret_cast<void *>(solve);
	entry->destroy = destroy;
}