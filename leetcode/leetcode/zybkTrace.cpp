#include "zybkTrace.h"
#include "ThreadPool.h"
#include <fstream>
#include <map>
#include <stack>
#include <string>
#include <windows.h>

LONG WINAPI __declspec(dllexport)  MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
    TraceManager::GetInstance()->dumpTrace();

    LOGD("ERROR!!!!!");
    return EXCEPTION_EXECUTE_HANDLER;
}
void dumpTrace()
{
    TraceManager::GetInstance()->dumpTrace();
}

TraceManager* TraceManager::mTraceManager=nullptr;
mutex TraceManager::mLock;
TraceManager *TraceManager::GetInstance()
{
    if (mTraceManager == nullptr)
    {
        unique_lock<mutex> lock(mLock);
        if(mTraceManager == nullptr){
            // cout << "set exception filter" << endl;
            SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
            mTraceManager = new TraceManager();
        }
    }
    return mTraceManager;
}
/**
       *  @brief  implement this function only to pass it to threadPool.
       *  @param  string  a string that record the call stack
       */
void stackTrace(string trace, unsigned int mthread)
{
    TraceManager::GetInstance()->addTrace(trace, mthread);
}
void unstackTrace(unsigned int mthread)
{
    TraceManager::GetInstance()->removeTrace(mthread);
}
TraceWrapper::TraceWrapper(string &&mtrace)
{
    unsigned int tid = (unsigned int)GetCurrentThreadId();
    // using mtype=decltype(stackTrace(string("move(mtrace)"),1));
    // auto mTask1 = std::make_shared<std::packaged_task<mtype()>>(bind((stackTrace), string("move(mtrace)"),1));
    // auto mTask = std::make_shared<std::packaged_task<mtype()>>(bind((stackTrace), "move(mtrace)",move(tid)));
    ThreadPoolManager::PostJob(stackTrace, "stackTrace", 8, mtrace, (tid)).get();
}
TraceWrapper::TraceWrapper(string &trace)
{
    unsigned int tid = (unsigned int)GetCurrentThreadId();
    ThreadPoolManager::PostJob(stackTrace, "stackTrace", 8, (trace), (tid)).get();
}
TraceWrapper::~TraceWrapper()
{
    ThreadPoolManager::PostJob(unstackTrace,
                                       "unstackTrace", 8, (unsigned int)GetCurrentThreadId()).get();
}
