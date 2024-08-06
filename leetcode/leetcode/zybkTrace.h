#ifndef _ZYBKTRACE_H_
#define _ZYBKTRACE_H_

#include "zybkLog.h"
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <stack>
// #include <string.h>
#include <ctime>
#include <string>
#include <windows.h>
#include <direct.h>

using namespace std;
// class __declspec(dllexport) TraceManager;
class __declspec(dllexport) TraceWrapper;

#define ZYBK_TRACE()                                                                                                    \
    TraceWrapper zybkTraceHELPPER(__FILE__, __func__, __LINE__)

class TraceManager
{
    static TraceManager *mTraceManager;
    static mutex mLock;
    map<int, stack<string>> mStackMap;

protected:
    TraceManager() = default;
    TraceManager(const TraceManager &) = delete;
    TraceManager operator=(const TraceManager &) = delete;

public:
    static TraceManager *GetInstance();

    void addTrace(string Trace, int mthread)
    {
        stack<string> sta;
        if (mStackMap.count(mthread) == 0)
        {

            mStackMap.insert({mthread, move(sta)});
        }
        mStackMap[mthread].push(Trace);
    }
    void addTrace(string &&Trace, int mthread)
    {
        stack<string> sta;
        if (mStackMap.count(mthread) == 0)
        {

            mStackMap.insert({mthread, sta});
        }
        mStackMap[mthread].push(Trace);
    }
    void removeTrace(int mthread)
    {
        LOG_DEBUG(mStackMap[mthread].top().c_str());
        mStackMap[mthread].pop();
    }
    void dumpTrace()
    {
        // backtrace();
        std::fstream fs;
        char path[1000];
        _getcwd(path, 1000);
        sprintf(path, "%s/backtrace.txt", path);   
        fs.open(path, std::fstream::in | std::fstream::out | std::fstream::app);
        for (auto itr : mStackMap)
        {
            while (itr.second.size() > 0)
            {
                string trace = itr.second.top();
                itr.second.pop();
                fs << trace << endl;
                // cout << trace << endl;
            }
        }

        fs.close();
    }
};

#define ZYBK_TRACE_DUMP() dumpTrace()

void __declspec(dllexport) dumpTrace();

class __declspec(dllexport) TraceWrapper
{
public:
    TraceWrapper(string &&mtrace, const char *, int);
    TraceWrapper(string &trace, const char *, int);
    ~TraceWrapper();
    TraceWrapper(const TraceWrapper &) = delete;
    TraceWrapper operator=(const TraceWrapper &) = delete;
};

string __declspec(dllexport)  getLastErrorFromCode();

#endif