// leetcode.cpp 

#include "algorithmFactory.h"
#include "maxSubArray.cpp"
#include "solutionEntry.h"
#include <Windows.h>
#include <deque>
#include <iostream>
// #include <libloaderapi.h>
#include <Windows.h>
#include"ThreadPool.h"
#include<iomanip>
#include"zybkLog.h"



int main()
{
	LOGD("start main function!");

	solutionEntry mSolution;
	EntryFunc mEntry;
	Solution sol;
	pHandle mHandle;
	int result;
	vector<int> myVec({83647});
	cout << sol.maxSubArray(myVec) << endl;
	HMODULE libHandle = LoadLibrary("C:/Users/zy113/vsProjects/libs/libzybk.kmp.dll");
	mEntry = reinterpret_cast<EntryFunc>(GetProcAddress(libHandle, "Entry"));
	mEntry(&mSolution);
	mSolution.create(mHandle);
	string str1 = string("zybkisfun !!!");
	string str2 = string("isfun !");

	//mSolution.solve(mHandle, mData, &result);
	mSolution.process(mHandle, str1, str2, result);
	mSolution.destroy(mHandle);
	FreeLibrary(libHandle);

	if (AlgorithmFactory::GetIntance()->getEntry("kmp", &mSolution) == 0)
	{
		mSolution.create(mHandle);
		string str3 = string("zybkisfun !!!");
		string str4 = string("isfun !");

		//mSolution.solve(mHandle, mData, &result);
		mSolution.process(mHandle, str3, str4, result);
		mSolution.destroy(mHandle);
	}

	system("pause");
	return 0;
}
