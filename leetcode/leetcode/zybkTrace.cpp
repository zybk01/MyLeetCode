#include "zybkTrace.h"
#include "ThreadPool.h"
#include <dbghelp.h>
#include <fstream>
#include <map>
#include <stack>
#include <string>
#include <windows.h>

/**
 *  @brief  call winAPI to dump register. memory and call stack info.
 *  @param  *ExceptionInfo
 *  call winAPI to dump register. memory and call stack info.
 *  however, cmake can't produce pdb file windgb tool required,
 *  just leave it alone
 */
// void minidump(struct _EXCEPTION_POINTERS *ExceptionInfo)
// {
//     CONTEXT c;

//     memset(&c, 0, sizeof(c));

//     GetThreadContext( GetCurrentThread(), &c );

//     EXCEPTION_POINTERS ep;

//     ep.ContextRecord = &c;
//     ep.ExceptionRecord = ExceptionInfo->ExceptionRecord;
//     // Open the file
//     char path[1000];
//     _getcwd(path, 1000);
//     sprintf(path, "%s/MiniDump.txt", path);
//     HANDLE hFile = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

//     if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
//     {
//         // Create the minidump

//         MINIDUMP_EXCEPTION_INFORMATION mdei;

//         mdei.ThreadId = GetCurrentThreadId();
//         mdei.ExceptionPointers = &ep;
//         mdei.ClientPointers = FALSE;

//         MINIDUMP_TYPE mdt = MiniDumpNormal;

//         BOOL rv = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
//                                     hFile, mdt, &mdei, 0, 0);

//         // Close the file

//         CloseHandle(hFile);
//     }
// }

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
