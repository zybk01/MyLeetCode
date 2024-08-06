#include "zybkTrace.h"
#include "ThreadPool.h"
#include <dbghelp.h>
#include <fstream>
#include <map>
#include <stack>
#include <string>
#include <tlhelp32.h>
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

string getLastErrorFromCode()
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);
    string  stringTmp(((LPCTSTR)lpMsgBuf));
    LocalFree(lpMsgBuf);
    return stringTmp;
}
void dumpMemoryRegion()
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    for (LPVOID addr = sysinfo.lpMinimumApplicationAddress; addr < sysinfo.lpMaximumApplicationAddress;)
    {
        MEMORY_BASIC_INFORMATION minfo;
        VirtualQuery(addr, &minfo, sizeof(MEMORY_BASIC_INFORMATION));
        IMAGEHLP_SYMBOL64 *pSym = (IMAGEHLP_SYMBOL64 *)malloc(sizeof(IMAGEHLP_SYMBOL64) + 1024);
        memset(pSym, 0, sizeof(IMAGEHLP_SYMBOL64) + 1024);
        pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
        pSym->MaxNameLength = 1024;
        SymGetSymFromAddr64(GetCurrentProcess(), (DWORD64)addr, 0, pSym);
        LOGD("%08x\t%08X\t%08X\r\n %s", addr, minfo.State, minfo.RegionSize, pSym->Name);
        LOG_DEBUG("%08x\t%08X\t%08X\r\n", addr, minfo.State, minfo.RegionSize);
        addr = (LPVOID)((byte *)addr + minfo.RegionSize);
        delete (pSym);
    }
}

void LoadModule()
{
    const size_t nTempLen = 1024;
    const size_t nSymPathLen = 4096;
    char szTemp[nTempLen];
    char szSymPath[4096];
    if (GetEnvironmentVariableA("PATH", szTemp, nTempLen) > 0)
    {
        szTemp[nTempLen - 1] = 0;
        strcat_s(szSymPath, nSymPathLen, szTemp);
        strcat_s(szSymPath, nSymPathLen, ";");
    }
    if (GetCurrentDirectoryA(nTempLen, szTemp) > 0)
    {
        szTemp[nTempLen - 1] = 0;
        strcat_s(szSymPath, nSymPathLen, szTemp);
        strcat_s(szSymPath, nSymPathLen, ";");
    }
    SymInitialize(GetCurrentProcess(), szSymPath, false);
    DWORD symOptions = SymGetOptions(); // SymGetOptions
    symOptions |= SYMOPT_LOAD_LINES;
    symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;
    symOptions |= SYMOPT_ALLOW_ABSOLUTE_SYMBOLS;
    symOptions |= SYMOPT_LOAD_ANYTHING;
    // symOptions |= SYMOPT_NO_PROMPTS;
    //  SymSetOptions
    symOptions = SymSetOptions(symOptions);
    MODULEENTRY32 me;
    me.dwSize = sizeof(me);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    BOOL keepGoing = !!Module32First(hSnap, &me);
    int cnt = 0;
    while (keepGoing)
    {
        //   this->LoadModule(hProcess, me.szExePath, me.szModule, (DWORD64)me.modBaseAddr,
        //                    me.modBaseSize);
        SymLoadModule64(GetCurrentProcess(), 0, me.szExePath, me.szModule, (DWORD64)me.modBaseAddr,
                        me.modBaseSize);
        //LOGD("SymLoadModule64 %s %s", me.szExePath, me.szModule);
        cnt++;
        keepGoing = !!Module32Next(hSnap, &me);
    }
}

LONG WINAPI __declspec(dllexport) MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
    TraceManager::GetInstance()->dumpTrace();

    LOG_DEBUG("ERROR!!!!!");
    {
        // IMAGEHLP_LINE64 errorLine;
        // memset(&errorLine, 0, sizeof(IMAGEHLP_LINE64));
        // errorLine.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        // errorLine.LineNumber = 0;
        // // without .pdb file ,can't resolve line information
        // SymGetLineFromAddr(GetCurrentProcess(), ExceptionInfo->ContextRecord->Rip, 0, &errorLine);
        // LOGD("ExceptionInfo %p ExceptionAddress %p rip %p FileName %s errorLine %d", ExceptionInfo, ExceptionInfo->ExceptionRecord->ExceptionAddress, ExceptionInfo->ContextRecord->Rip, errorLine.FileName, errorLine.LineNumber);
        // char cleanName[1024];
        // UnDecorateSymbolName(pSym->Name, cleanName, 1024, UNDNAME_NAME_ONLY);
        // LOGD("SymGetSymFromAddr64 %s Name %s", pSym->Name, cleanName);
        // UnDecorateSymbolName(pSym->Name, cleanName, 1024, UNDNAME_COMPLETE);
    }
    LOGD("ExceptionCode %x Rax %x ", ExceptionInfo->ExceptionRecord->ExceptionCode, ExceptionInfo->ContextRecord->Rax);
    LoadModule();
    IMAGEHLP_SYMBOL64 *pSym = (IMAGEHLP_SYMBOL64 *)malloc(sizeof(IMAGEHLP_SYMBOL64) + 1024);
    memset(pSym, 0, sizeof(IMAGEHLP_SYMBOL64) + 1024);
    pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
    pSym->MaxNameLength = 1024;
    SymGetSymFromAddr64(GetCurrentProcess(), ExceptionInfo->ContextRecord->Rip, 0, pSym);
    IMAGEHLP_MODULE64 Module;
    memset(&Module, 0, sizeof(IMAGEHLP_MODULE64));
    Module.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
    SymGetModuleInfo64(GetCurrentProcess(), ExceptionInfo->ContextRecord->Rip, &Module);
    LOGD("ExceptionInfo %p ExceptionAddress %p SymGetSymFromAddr64 %s Name %s base %p", ExceptionInfo, ExceptionInfo->ExceptionRecord->ExceptionAddress, pSym->Name, Module.ImageName, Module.BaseOfImage);
    LOG_DEBUG("ExceptionInfo %p ExceptionAddress %p SymGetSymFromAddr64 %s Name %s base %p", ExceptionInfo, ExceptionInfo->ExceptionRecord->ExceptionAddress, pSym->Name, Module.ImageName, Module.BaseOfImage);
    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(STACKFRAME64));
    EXCEPTION_INVALID_DISPOSITION;
    while(1) {
        CONTEXT c;
        memset(&c, 0, sizeof(c));
        GetThreadContext(GetCurrentThread(), &c);
        IMAGEHLP_MODULE64 Module;
        memset(&Module, 0, sizeof(IMAGEHLP_MODULE64));
        Module.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
        if(!StackWalk64(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), GetCurrentThread(), &stackFrame, &c, 0, 0, 0, 0)) {
            LOGD("errorCode(%d):%s", GetLastError(), getLastErrorFromCode().c_str());
            break;
        }
        SymGetSymFromAddr64(GetCurrentProcess(), stackFrame.AddrPC.Offset, 0, pSym);
        SymGetModuleInfo64(GetCurrentProcess(), stackFrame.AddrPC.Offset, &Module);
        if (stackFrame.AddrPC.Offset == stackFrame.AddrReturn.Offset) {
            break;
        }
        LOGD("SymGetSymFromAddr64 %s Name %s %s", pSym->Name, Module.ImageName, Module.ModuleName);
    }
    void* stack[8];
    WORD frames = CaptureStackBackTrace(8, 16, stack, NULL);
    for (int i = 0; i < frames; i++)
    {
        IMAGEHLP_MODULE64 Module;
        memset(&Module, 0, sizeof(IMAGEHLP_MODULE64));
        Module.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

        SymGetSymFromAddr64(GetCurrentProcess(), (DWORD64)stack[i], 0, pSym);
        SymGetModuleInfo64(GetCurrentProcess(),  (DWORD64)stack[i], &Module);
        LOGD("SymGetSymFromAddr64 %s Name %s %s", pSym->Name, Module.ImageName, Module.ModuleName);
    }
    delete (pSym);
    char command[1024];
    snprintf(command, sizeof(command), "addr2line -Cfe %s -i %p", Module.ImageName, ExceptionInfo->ExceptionRecord->ExceptionAddress);
    LOGD("%s", command);
    system(command);
    // dumpMemoryRegion();
    // ThreadPoolManager::CheckOut();
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
