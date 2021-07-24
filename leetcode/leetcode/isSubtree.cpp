#include "stdafx.h"
#include<vector>
#include<iostream>
#include<algorithm>

using namespace std;


//Definition for a binary tree node.
 struct TreeNode {
     int val;
     TreeNode *left;
     TreeNode *right;
     TreeNode() : val(0), left(nullptr), right(nullptr) {}
     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 };


class Solution {
public:
	bool isSubtree(TreeNode* s, TreeNode* t) {
		if (s==NULL)
		{
			return false;
		}
		if (isEqualTree(t, s))
		{
			return true;
		}
		else {
			return isSubtree(s->left, t) || isSubtree(s->right, t);
		}
	}
	bool isEqualTree(TreeNode *t, TreeNode *s) {
		if (t==NULL&&s==NULL)
		{
			return true;
		}
		else if (t == NULL||s == NULL)
		{
			return false;
		}
		else if (t->val==s->val)
		{
			return isEqualTree(t->left, s->left) && isEqualTree(t->right, s->right);
		}
		else {
			return false;
		}
	}
};