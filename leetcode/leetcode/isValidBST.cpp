// #include "stdafx.h"
#include "isValidBST.h"
#include "solutionEntry.h"
#include "zybkLog.h"
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

//* Definition for a binary tree node.
// struct TreeNode {
// 	int val;
// 	TreeNode *left;
// 	TreeNode *right;
// 	TreeNode(int x) : val(x), left(NULL), right(NULL) {}
// };
bool SolutionIsValidBST::isValidBST(TreeNode *root)
{
	return verify(root, 0x7fffffff, -0x7fffffff);
}
bool SolutionIsValidBST::verify(TreeNode *root, int L, int S)
{
	if (root == NULL)
	{
		return true;
	}
	else if (root->val < S || root->val > L)
	{
		return false;
	}
	else
	{
		return verify(root->left, root->val, S) && verify(root->right, L, root->val);
	}
}

extern "C" __declspec(dllexport) void Entry(solutionEntryBase *entry);

void create(pHandle &out)
{
	LOGD("Created!");
	out = reinterpret_cast<pHandle>(new SolutionIsValidBST);
}

void solve(pHandle handle, TreeNode *&input, bool &out)
{
	LOGD("Solving!");
	auto *pSolution = reinterpret_cast<SolutionIsValidBST *>(handle);
	out = pSolution->isValidBST(input);
	return;
}
void destroy(pHandle handle)
{
	LOGD("Destroyed!");
	delete reinterpret_cast<SolutionIsValidBST *>(handle);
}

void Entry(solutionEntryBase *entry)
{
	LOGD("Get Entry!");
	entry->create = create;
	entry->solve = reinterpret_cast<void *>(solve);
	entry->destroy = destroy;
}