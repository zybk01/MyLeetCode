#include"stdafx.h"
#include<vector>
#include<algorithm>
#include<queue>
#include<iostream>
#include<string>
#include"solutionEntry.h"

//#define SHARED_EXPORT __declspec(dllexport)
using namespace std;

struct Solution;


class Solution {
public:
	int kmp(string &T,string &P) {
		int m = T.length();
		int n = P.length();
		vector<int> prefix = computePrefix(P);
		int k = 0;
		int res = 0;
		for (int i = 0; i < m; i++)
		{
			while (k>0&&P[k] != T[i])
			{
				k = prefix[k-1];
			}
			if (P[k] == T[i]) {
				k++;
			}
			if (k==n)
			{
				k = prefix[k - 1];
				res++;
			}
		}
		return res;

	}
	vector<int> computePrefix(string&P) {
		int len = P.length();
		vector<int> prefix;
		prefix.push_back(0);
		int k = 0;
		for (int i = 1; i < len; i++)
		{
			
			while (k>0&& P[i] != P[k])
			{
				k = prefix[k-1];
			}
			if (P[i] == P[k]) {
				k++;
			}
			prefix.push_back(k);
		}
		return prefix;
	}
};

extern "C" __declspec(dllexport) void Entry(solutionEntryBase *entry);

void create(pHandle& out){
	cout << __FILE__ << ": created!" << endl;
	out = reinterpret_cast<pHandle>(new Solution);
}
// void solve(pHandle handle,pData dataptr,void *out){
// 	Solution* pSolution=reinterpret_cast<Solution*>(handle);
// 	int result= pSolution->kmp(*(reinterpret_cast<string*>(dataptr[0])),*(reinterpret_cast<string*>(dataptr[1])));
// 	*reinterpret_cast<int*>(out) = result;
// }
void solve(pHandle handle,string& str1,string& str2 ,int& out){
	cout << __FILE__ << ": solving!" << endl;
	cout << __FILE__ <<" str1  ="<<str1<<" str2  ="<< str2<<endl;
	Solution* pSolution=reinterpret_cast<Solution*>(handle);
	int result= pSolution->kmp(str1,str2);
	cout << __FILE__ << ": result = " <<result<< endl;
	out = result;
	return;
}
void destroy(pHandle handle){
	cout << __FILE__ << ": destroyed!" << endl;
	delete reinterpret_cast<Solution*>(handle);
}

void Entry(solutionEntryBase *entry){
	cout << __FILE__ << " get entry!" << endl;
	entry->create = create;
	entry->solve = reinterpret_cast<void*>(solve);
	// entry->solve = solve;
	entry->destroy = destroy;
}


//
//int main()
//{
//	Solution sol;
//
//	//sol.computePrefix(string("ababaca") );
//
//
//	cout << sol.kmp(string("bacbababacaabcbab"), string("ab")) << endl;
//	return 0;
//}