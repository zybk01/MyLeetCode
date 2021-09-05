#ifndef _ZYBKTRACE_H_
#define _ZYBKTRACE_H_

// #include "ThreadPool.h"
#include <fstream>
#include <map>
#include <mutex>
#include <stack>
// #include <string.h>
#include <ctime>
#include <string>
#include <windows.h>

using namespace std;
// class __declspec(dllexport) TraceManager;
class __declspec(dllexport) TraceWrapper;

#define ZYBK_TRACE()                                                                                             \
    char *ZYBKTRACE_message = new char[1000];                                                                    \
    int ZYBKTRACE_i = 0;                                                                                         \
    time_t ZYBKTRACE_time = time(nullptr);                                                                       \
    *ZYBKTRACE_message = '\0';                                                                                   \
    ZYBKTRACE_i += sprintf(ZYBKTRACE_message + ZYBKTRACE_i, "%-22ld%-20s:%-20s:%-10d  processId:%d  threadId:%d", \
                           ZYBKTRACE_time, RMPATHr(RMPATH(__FILE__)), __func__, __LINE__,                \
                           (unsigned int)GetCurrentProcessId(), (unsigned int)GetCurrentThreadId());             \
    TraceWrapper zybkTraceHELPPER(ZYBKTRACE_message);                                                            \
    do                                                                                                           \
    {                                                                                                            \
        delete[] ZYBKTRACE_message;                                                                              \
    } while (0)

class TraceManager
{
    static TraceManager *mTraceManager;
    static mutex mLock;
    //stack<string> mTraceStack;
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
        // cout<<mStackMap[thread].size()<<endl;
        // mTraceStack.push(Trace);
    }
    void addTrace(string &&Trace, int mthread)
    {
        stack<string> sta;
        if (mStackMap.count(mthread) == 0)
        {

            mStackMap.insert({mthread, sta});
        }
        mStackMap[mthread].push(Trace);

        // mTraceStack.push(Trace);
    }
    void removeTrace(int mthread)
    {
        mStackMap[mthread].pop();
        // LOGD("~%d", mStackMap[mthread].size());
        //  cout<<"~"<<mStackMap[thread].size()<<endl;
        // mTraceStack.pop();
    }
    void dumpTrace()
    {
        std::fstream fs;
        fs.open("C:\\Users\\zy113\\vsProjects\\test.txt", std::fstream::in | std::fstream::out | std::fstream::app);
        // LOGD("%d",mStackMap.size());
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
    TraceWrapper(string &&mtrace);
    TraceWrapper(string &trace);
    ~TraceWrapper();
    TraceWrapper(const TraceWrapper &) = delete;
    TraceWrapper operator=(const TraceWrapper &) = delete;
};

#endif