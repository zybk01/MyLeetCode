#include "stdafx.h"
#include<vector>
#include<iostream>
#include<algorithm>

using namespace std;



//* Definition for a binary tree node.
struct TreeNode {
	int val;
	TreeNode *left;
	TreeNode *right;
	TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
public:
	bool isValidBST(TreeNode* root) {
		return verify(root,0x7fffffff,-0x7ffffffff);
	}
	bool verify(TreeNode* root,int L,int S) {
		if (root==NULL)
		{
			return true;
		}
		else if(root->val<S||root->val>L)
		{
			return false;
		}
		else
		{
			return verify(root->left, root->val, S) && verify(root->right, L, root->val);
		}
	}
};