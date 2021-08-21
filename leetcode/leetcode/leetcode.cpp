// leetcode.cpp : �������̨Ӧ�ó������ڵ㡣
//

//#include "stdafx.h"
#include "maxSubArray.cpp"
#include "solutionEntry.h"
#include <Windows.h>
#include <deque>
#include <iostream>
#include <libloaderapi.h>

int main()
{
	solutionEntry mSolution;
	EntryFunc mEntry;
	Solution sol;
	pHandle mHandle;
	int result;
	vector<int> myVec({83647});
	cout << sol.maxSubArray(myVec) << endl;
	HMODULE libHandle = LoadLibrary("C:/Users/zy113/vsProjects/libzybk.kmp.dll");
	cout <<__FILE__<< ": library!!!" << endl;
	mEntry = reinterpret_cast<EntryFunc>(GetProcAddress(libHandle, "Entry"));
	cout <<__FILE__<< ": interface!!!" << endl;
	mEntry(&mSolution);
	cout <<__FILE__<< ": method!!!" << endl;
	mSolution.create(mHandle);
	string str1 = string("zybkisfun !!!");
	string str2 = string("isfun !");

	//mSolution.solve(mHandle, mData, &result);
	mSolution.process(mHandle, str1, str2, result);
	cout <<__FILE__<< ": mResult = " << result << endl;
	mSolution.destroy(mHandle);
	FreeLibrary(libHandle);
	
	cout <<__FILE__<< ": Done!!!" << endl;
	system("pause");
	return 0;
}
