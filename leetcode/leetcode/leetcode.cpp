// leetcode.cpp : �������̨Ӧ�ó������ڵ㡣
//

//#include "stdafx.h"
#include<iostream>
#include<Windows.h>
#include"maxSubArray.cpp"
#include<deque>
#include<libloaderapi.h>
#include"solutionEntry.h"




int main()
{
	solutionEntry mSolution;
	EntryFunc mEntry;
	Solution sol;
	pHandle mHandle;
	int result;
	vector<int> myVec({ 83647 });
	cout << sol.maxSubArray(myVec) << endl;
	HMODULE libHandle = LoadLibrary("c:/Users/zy113/vsProjects/build/leetcode/leetcode/libzybk.kmp.dll");
	mEntry=reinterpret_cast<EntryFunc>(GetProcAddress(libHandle,"Entry"));
	mEntry(mSolution);
	mSolution.create(mHandle);
	void *mData[] = {(void *)new string("zybkisfun !!!"), (void*)new string("isfun !")};
	mSolution.solve(mHandle, mData, &result);
	mSolution.destroy(mHandle);
	FreeLibrary(libHandle);
	cout << "mResult = " << result << endl;
	cout << "Done!!!" << endl;
	system("pause");
	return 0;
}

