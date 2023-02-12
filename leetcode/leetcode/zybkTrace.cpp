#include "zybkTrace.h"
#include "ThreadPool.h"
#include <fstream>
#include <map>
#include <stack>
#include <string>
#include <windows.h>

LONG WINAPI __declspec(dllexport) MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
    TraceManager::GetInstance()->dumpTrace();

    LOG_DEBUG("ERROR!!!!!");
    return EXCEPTION_EXECUTE_HANDLER;
}
void dumpTrace()
{
    TraceManager::GetInstance()->dumpTrace();
}

TraceManager *TraceManager::mTraceManager = nullptr;
mutex TraceManager::mLock;
TraceManager *TraceManager::GetInstance()
{
    if (mTraceManager == nullptr)
    {
        unique_lock<mutex> lock(mLock);
        if (mTraceManager == nullptr)
        {
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
TraceWrapper::TraceWrapper(string &&filename, const char *func, int line)
{
    char *ZYBKTRACE_message = new char[1000];
    int ZYBKTRACE_i = 0;
    *ZYBKTRACE_message = '\0';
    SYSTEMTIME ZYBKTRACE_time;
    GetLocalTime(&ZYBKTRACE_time);
    time_t ZYBKTRACE_time1 = time(nullptr);
    ZYBKTRACE_i += sprintf(ZYBKTRACE_message + ZYBKTRACE_i, "%4d/%02d/%02d %02d:%02d:%02d.%03d ", ZYBKTRACE_time.wYear,
                           ZYBKTRACE_time.wMonth, ZYBKTRACE_time.wDay, ZYBKTRACE_time.wHour, ZYBKTRACE_time.wMinute,
                           ZYBKTRACE_time.wSecond, ZYBKTRACE_time.wMilliseconds);
    ZYBKTRACE_i += sprintf(ZYBKTRACE_message + ZYBKTRACE_i, "%d %d:  ", (unsigned int)GetCurrentProcessId(), (unsigned int)GetCurrentThreadId());
    ZYBKTRACE_i += sprintf(ZYBKTRACE_message + ZYBKTRACE_i, "%-20s:%-20sLINE:%-10d",
                           RMPATHr(RMPATH(filename.c_str())), func, line);
    unsigned int tid = (unsigned int)GetCurrentThreadId();
    // using mtype=decltype(stackTrace(string("move(mtrace)"),1));
    // auto mTask1 = std::make_shared<std::packaged_task<mtype()>>(bind((stackTrace), string("move(mtrace)"),1));
    // auto mTask = std::make_shared<std::packaged_task<mtype()>>(bind((stackTrace), "move(mtrace)",move(tid)));
    auto postStatus = ThreadPoolManager::PostJob(stackTrace, "stackTrace", THREAD_TYPE_TRACE, (string(ZYBKTRACE_message)), (tid));
    if (postStatus.second == threadStatus::THREAD_SUBMIT)
        postStatus.first.get();
}
TraceWrapper::TraceWrapper(string &filename, const char *func, int line)
{
    char *ZYBKTRACE_message = new char[1000];
    int ZYBKTRACE_i = 0;
    *ZYBKTRACE_message = '\0';
    SYSTEMTIME ZYBKTRACE_time;
    GetLocalTime(&ZYBKTRACE_time);
    time_t ZYBKTRACE_time1 = time(nullptr);
    ZYBKTRACE_i += sprintf(ZYBKTRACE_message + ZYBKTRACE_i, "%4d/%02d/%02d %02d:%02d:%02d.%03d ", ZYBKTRACE_time.wYear,
                           ZYBKTRACE_time.wMonth, ZYBKTRACE_time.wDay, ZYBKTRACE_time.wHour, ZYBKTRACE_time.wMinute,
                           ZYBKTRACE_time.wSecond, ZYBKTRACE_time.wMilliseconds);
    ZYBKTRACE_i += sprintf(ZYBKTRACE_message + ZYBKTRACE_i, "%d %d:  ", (unsigned int)GetCurrentProcessId(), (unsigned int)GetCurrentThreadId());
    ZYBKTRACE_i += sprintf(ZYBKTRACE_message + ZYBKTRACE_i, "%-20s:%-20sLINE:%-10d",
                           RMPATHr(RMPATH(filename.c_str())), func, line);
    unsigned int tid = (unsigned int)GetCurrentThreadId();
    auto postStatus = ThreadPoolManager::PostJob(stackTrace, "stackTrace", THREAD_TYPE_TRACE, (string(ZYBKTRACE_message)), (tid));
    if (postStatus.second == threadStatus::THREAD_SUBMIT)
        postStatus.first.get();
}
TraceWrapper::~TraceWrapper()
{
    auto postStatus = ThreadPoolManager::PostJob(unstackTrace,
                                                 "unstackTrace", THREAD_TYPE_TRACE, (unsigned int)GetCurrentThreadId());
    if (postStatus.second == threadStatus::THREAD_SUBMIT)
        postStatus.first.get();
            LOG_DEBUG("unstackTraces");
}
