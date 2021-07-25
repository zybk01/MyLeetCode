#include"stdafx.h"
#include<vector>
#include<algorithm>
#include<queue>
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

extern "C" __declspec(dllexport) void Entry(solutionEntry &entry);

void create(pHandle& out){
	out = reinterpret_cast<pHandle>(new Solution);
}
void solve(pHandle handle,pData dataptr,void *out){
	Solution* pSolution=reinterpret_cast<Solution*>(handle);
	int result= pSolution->kmp(*(reinterpret_cast<string*>(dataptr[0])),*(reinterpret_cast<string*>(dataptr[1])));
	*reinterpret_cast<int*>(out) = result;
}
void destroy(pHandle handle){
	delete reinterpret_cast<Solution*>(handle);
}

void Entry(solutionEntry &entry){
	entry.create = create;
	entry.solve = solve;
	entry.destroy = destroy;
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