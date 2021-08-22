// #include "stdafx.h"
#include<vector>
#include<iostream>
#include<algorithm>
#include"zybkLog.h"
#include"solutionEntry.h"

using namespace std;



//* Definition for a binary tree node.
struct TreeNode {
	int val;
	TreeNode *left;
	TreeNode *right;
	TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class __declspec(dllexport) SolutionIsValidBST {
	
public:
    bool isValidBST(TreeNode *root);
protected:
    bool verify(TreeNode *root, int L, int S);
};



// extern "C" __declspec(dllexport) void Entry(solutionEntryBase *entry);

// void create(pHandle &out);
// void solve(pHandle handle, TreeNode *&input, bool &out);
// void destroy(pHandle handle);
// void Entry(solutionEntryBase *entry);
