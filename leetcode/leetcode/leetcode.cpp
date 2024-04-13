// leetcode.cpp

#include "algorithmFactory.h"
#include "maxSubArray.cpp"
#include "treeOfInfiniteSouls.h"
#include "solutionEntry.h"
#include <Windows.h>
#include <deque>
#include <iostream>
// #include <libloaderapi.h>
#include "ThreadPool.h"
#include "zybkLog.h"
#include "zybkTrace.h"
#include <Windows.h>
#include <iomanip>

int main()
{
	ZYBK_TRACE();
	LOGD("start main function!");

	solutionEntry mSolution;
	EntryFunc mEntry;
	Solution sol;
	pHandle mHandle;
	int result;
	vector<int> myVec({83647});
	cout << sol.maxSubArray(myVec) << endl;
	HMODULE libHandle = LoadLibrary("D:/repo/MyLeetCode/libs/libzybk.kmp.dll");
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
	SolutiontreeOfInfiniteSouls sloutu;
	// vector<int> gem({3,21,3});
	// LOGD("treeOfInfiniteSouls %d ", sloutu.treeOfInfiniteSouls(gem, 7, 5));
	// vector<int> gem({636069358,43941083});
	// LOGD("treeOfInfiniteSouls %d ", sloutu.treeOfInfiniteSouls(gem, 4421, 2509));
	// vector<int> gem({87821,70624});
	// LOGD("treeOfInfiniteSouls %d ", sloutu.treeOfInfiniteSouls(gem, 23178401, 8061209));
	// vector<int> gem({764,22,24,379,220,515});
	// LOGD("treeOfInfiniteSouls %d ", sloutu.treeOfInfiniteSouls(gem, 360117497, 223705638));
	vector<int> gem({71,39,50});
	LOGD("treeOfInfiniteSouls %d ", sloutu.treeOfInfiniteSouls(gem, 47, 1));
	system("pause");
	return 0;
}
