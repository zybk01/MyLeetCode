#include "stdafx.h"
#include<string>
#include<algorithm>
#include<unordered_map>
#include<iostream>
using namespace std;


class Solution {
public:
	int getMaxRepetitions(string s1, int n1, string s2, int n2) {
		int len1 = s1.length();
		int len2 = s2.length();
		int ptr = 0;
		int i = 0;
		int mark1 = 0;
		int knotadd = 0;
		unordered_map<int,pair<int,int>> map;
		for (;;)
		{
			if (i >= len1*n1)
			{
				return ptr / len2 / n2;
			}
			if (i > (len1*(len2 + 1))) {
				//cout << i << endl;
			}
			if (s1[(i%len1)] == s2[(ptr%len2)]) {
				if (ptr == 0)
				{
					map[i%len1] = { i,ptr };
					mark1 = i;
				}
				else if (ptr%len2 == 0) 
				{
					if (map.count(i%len1)) {
						mark1=(n1*len1-i)/(i - map[i%len1].first)*(ptr - map[i%len1].second)+ map[i%len1].second;
						knotadd= ((n1*len1 - i) / (i - map[i%len1].first)-1)* (i - map[i%len1].first);
						break;
					}
					else
					{
						map[i%len1] = { i,ptr };
					}
					//return (n1*(ptr / len2)*len1/ ((i - mark1))/n2 );

					//knotadd = ((n1*len1 - mark1) / (i - mark1) - 1)*(i - mark1);
					//mark1 = (n1*len1 - mark1) / (i - mark1) *(ptr / len2);
					//break;

				}

				ptr++;
			}
			i++;
		}
		ptr = 0;
		i = i + knotadd;

		for (;;)
		{
			if (i >= len1*n1)
			{
				return (mark1 + ptr) / len2 / n2;
			}
			if (s1.at(i%len1) == s2.at(ptr%len2)) {
				ptr++;
			}
			i++;
		}

		return 1;

	}
};




//int main()
//{
//	Solution sol;
//	//cout<<sol.getMaxRepetitions("lovelive", 10000, "lovelive", 10000)<<endl;
//
//
//
//
//	cout << sol.getMaxRepetitions("phqghumeaylnlfdxfircvscxggbwkfnqduxwfnfozvsrtkjprepggxrpnrvystmwcysyycqpevikeffmznimkkasvwsrenzkycxf", 1000000, "xtlsgypsfadpooefxzbcoejuvpvaboygpoeylfpbnpljvrvipyamyehwqnqrqpmxujjloovaowuxwhmsncbxcoksfzkvatxdknly", 100) << endl;
//	return 0;
//}