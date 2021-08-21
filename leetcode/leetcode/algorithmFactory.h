#ifndef _ALGORITHMFACTORY_H_
#define _ALGORITHMFACTORY_H_

#define kmpPath "C:/Users/zy113/vsProjects/libzybk.kmp.dll"
#include "solutionEntry.h"
#include <Windows.h>
#include <iostream>

#include <map>
#include <mutex>
#include <string>

using namespace std;

class AlgorithmFactory
{
    map<string, pair<HMODULE, solutionEntryBase>> mAlgoMap;
    mutex mOpenLock;
    mutex mGetLock;

protected:
    AlgorithmFactory(){
        LOGD(" ");
    };
    AlgorithmFactory(const AlgorithmFactory &);
    AlgorithmFactory operator=(const AlgorithmFactory &);
    int openAlgoLib(string libname)
    {
        auto lock = unique_lock<mutex>(mOpenLock);
        {
            auto lock = unique_lock<mutex>(mGetLock);
            if (mAlgoMap.count(libname) > 0)
            {
                return 0;
            }
        }
        solutionEntryBase mSolution;
        EntryFunc mEntry;
        HMODULE libHandle = LoadLibrary(libname.c_str());
        if (libHandle == NULL)
        {
            return -1;
        }
        LOGD(" open library, Path= %s",libname);
        mEntry = reinterpret_cast<EntryFunc>(GetProcAddress(libHandle, "Entry"));
        if (mEntry == NULL)
        {
            return -1;
        }
        mEntry(&mSolution);
        {
            auto lock = unique_lock<mutex>(mGetLock);

            mAlgoMap[libname] = make_pair(libHandle, mSolution);
        }
        return 0;
    }
    int mGetEntry(string libname, solutionEntryBase *entry)
    {
        {
            auto lock = unique_lock<mutex>(mGetLock);
            if (mAlgoMap.count(libname) > 0)
            {
                *entry = mAlgoMap[libname].second;
                return 0;
            }
        }

        if (openAlgoLib(libname) != 0)
        {
            return -1;
        }
        {
            auto lock = unique_lock<mutex>(mGetLock);
            if (mAlgoMap.count(libname) > 0)
            {
                *entry = mAlgoMap[libname].second;
            }
        }
        return 0;
    }

public:
    static AlgorithmFactory *GetIntance()
    {
        static AlgorithmFactory *mInstance = new AlgorithmFactory();
        return mInstance;
    }

    int getEntry(string libname, solutionEntryBase *entry)
    {
        LOGD(" get library entry, Algo= %s",libname);
        int ret = 0;
        if (libname.compare("kmp") == 0)
        {
            ret = mGetEntry(kmpPath, entry);
        }
        else
        {
            return -1;
        }
        return ret;
    }
    ~AlgorithmFactory()
    {
        LOGD(" ");
        auto lock = unique_lock<mutex>(mGetLock);
        for (auto itr : mAlgoMap)
        {
            FreeLibrary(itr.second.first);
        }
    }
};

#endif