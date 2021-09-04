#include "zybkTrace.h"
#include "ThreadPool.h"
#include <fstream>
#include <map>
#include <stack>
#include <string>
#include <windows.h>

TraceManager *TraceManager::GetInstance()
{
    static TraceManager *mTraceManager = new TraceManager();
    return mTraceManager;
}
    /**
       *  @brief  implement this function only to pass it to threadPool.
       *  @param  string  a string that record the call stack
       */
int stackTrace(string trace, unsigned int mthread)
{
    TraceManager::GetInstance()->addTrace(trace, mthread);
    return 0;
}
void unstackTrace(unsigned int mthread)
{
    TraceManager::GetInstance()->removeTrace(mthread);
}
TraceWrapper::TraceWrapper(string &&mtrace)
{
    unsigned int tid=(unsigned int)GetCurrentThreadId();
    // using mtype=decltype(stackTrace(string("move(mtrace)"),1));
    // auto mTask1 = std::make_shared<std::packaged_task<mtype()>>(bind((stackTrace), string("move(mtrace)"),1));
    // auto mTask = std::make_shared<std::packaged_task<mtype()>>(bind((stackTrace), "move(mtrace)",move(tid)));
    ThreadPool::GetInstance()->PostJob(stackTrace, "stackTrace", 8, mtrace, (tid));
}
TraceWrapper::TraceWrapper(string &trace)
{
    unsigned int tid=(unsigned int)GetCurrentThreadId();
    ThreadPool::GetInstance()->PostJob(stackTrace, "stackTrace", 8,(trace), (tid));
}
TraceWrapper::~TraceWrapper()
{
    ThreadPool::GetInstance()->PostJob(unstackTrace,
                                       "unstackTrace", 8, (unsigned int)GetCurrentThreadId());
}
