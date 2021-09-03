#include "ThreadPool.h"
#include "isValidBST.h"
#include "kmp.h"
#include "solutionEntry.h"
#include "test.h"
// #include <minwindef.h>
// #include <dbghelp.h>
// #include <excpt.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <windows.h>
// #include<debugapi.h>

LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
    // HANDLE lhDumpFile = CreateFile(("DumpFile.dmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);

    // MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
    // loExceptionInfo.ExceptionPointers = ExceptionInfo;
    // loExceptionInfo.ThreadId = GetCurrentThreadId();
    // loExceptionInfo.ClientPointers = TRUE;
    // MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),lhDumpFile, MiniDumpNormal, &loExceptionInfo, NULL, NULL);

    // CloseHandle(lhDumpFile);
    // std::fstream fs;
    // fs.open("C:\\Users\\zy113\\vsProjects\\test.txt", std::fstream::in | std::fstream::out | std::fstream::app);
    TraceManager::GetInstance()->dumpTrace();
    LOGD("ERROR!!!!!");

    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int, char **)
{

    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
    ZYBK_TRACE();
    // TraceWrapper(string(__DATE__) + string(__TIME__) + string(RMPATHr(RMPATH(__FILE__))) + string(__func__) + to_string(__LINE__));
    // char a[5];
    // char b = a[6];
    // vector<int> d(3);
    // int f = d[7];
    // cout << f << endl;
    // int *g = nullptr;
    // *g = 1;
    // float c = 1 / 0;
    // Derive D;
    // Base B;
    // // D.func1();
    // // D.func2();
    // // D.Base::func1();
    // D.Base::func2();
    // // D.func2();
    // B.func2();
    // Derive *pD = new Derive();
    // pD->func1();
    // // pD->Base::func1();
    // Base *pDb = &D;
    // auto funn = [](Base *b)
    // {
    //     b->func2();
    //     b->func1();
    // };
    // funn(pD);
    // funn(pDb);

    // std::thread t(fun);
    // std::cout << "Hello, world!\n";
    SolutionIsValidBST().isValidBST(new TreeNode(0));
    // string str1 = string("zybkisfun !!!");
    // string str2 = string("isfun !");
    // SolutionKMP().kmp(str1, str2);
    // fun();
    // t.join();
    // //auto mFuture1=ThreadPool(10).enqueue(fun);
    // //auto mFuture=ThreadPool(10).enqueue(std::move(&fun));
    // LOGD("try");
    auto mFuture = ThreadPool::GetInstance()->PostJob([]()
                                                      {
                                                          ZYBK_TRACE();
                                                          LOGD("try");
                                                          char a[5];
                                                          char b = a[6];
                                                          vector<int> d(3);
                                                          int f = d[7];
                                                          cout << f << endl;
                                                          int *g = nullptr;
                                                          *g = 1;

                                                          return;
                                                      },
                                                      "testTask", 9);
    auto mFuture1 = ThreadPool::GetInstance()->PostJob([](int num) -> int
                                                       {
                                                           // std::cout << "args threadTask = " << num << std::endl;
                                                           return num;
                                                       },
                                                       "testTask2", 1, 10);
    if (mFuture.valid())
    {
        mFuture.get();
        // mFuture.get();
        // std::cout << "no args threadTask" << std::endl;
    }
    if (mFuture1.valid())
    {
        mFuture1.get();
        // std::cout << "args threadTask = " << mFuture1.get() << std::endl;
    }
    // LOGD("Done");
    //
    auto checkoutsignal = ThreadPool::GetInstance()->checkOut();
    checkoutsignal.get();
    // system("pause");
    return 0;
}
