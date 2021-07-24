#include"stdafx.h"
#include<vector>
#include<algorithm>
#include<queue>
#include<string>
#include<stack>


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
	vector<int> rightSideView(TreeNode* root) {
		queue<TreeNode*> que;
		vector<int> vec;
		if (root == nullptr)
		{
			return vec;
		}
		que.push(root);
		while (!que.empty())
		{
			TreeNode* node;
			int len = que.size();
			for (int i = 0; i < len; i++)
			{

				node = que.front();
				que.pop();

				if (node->left != nullptr)
				{
					que.push(node->left);
				}
				if (node->right != nullptr)
				{
					que.push(node->right);
				}
			}

			vec.push_back(node->val);


		}
		return vec;


	}
};
