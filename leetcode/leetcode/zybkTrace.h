#ifndef _ZYBKTRACE_H_
#define _ZYBKTRACE_H_

#include "ThreadPool.h"
#include <fstream>
#include <map>
#include <stack>
#include <string.h>
#include <string>

// class __declspec(dllexport) TraceManager;
class __declspec(dllexport) TraceWrapper;

#define ZYBK_TRACE()                                                                                                                \
    char *message = new char[1000];                                                                                                 \
    int i = 0;                                                                                                                      \
    *message = '\0';                                                                                                                \
    i += sprintf(message + i, "%-12s%-10s%-20s:%-20s:%-10d:  threadId:%d", __DATE__, __TIME__, RMPATHr(RMPATH(__FILE__)), __func__, __LINE__,(unsigned int)GetCurrentThreadId()); \
    TraceWrapper zybkTraceHELPPER(message);                                                                                         \
    do                                                                                                                              \
    {                                                                                                                               \
        delete[] message;                                                                                                           \
    } while (0)

class TraceManager
{
    stack<string> mTraceStack;
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