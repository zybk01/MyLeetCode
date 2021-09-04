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
#include "zybkTrace.h"
// #include<debugapi.h>

LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
    TraceManager::GetInstance()->dumpTrace();
    
    LOGD("ERROR!!!!!");
    return EXCEPTION_EXECUTE_HANDLER;
}


int main(int, char **)
{
    ZYBK_TRACE();
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

    SolutionIsValidBST().isValidBST(new TreeNode(0));
    
    auto mFuture = ThreadPool::GetInstance()->PostJob([]()
                                                      {
                                                          ZYBK_TRACE();
                                                          LOGD("try");
                                                        //   char a[5];
                                                        //   char b = a[6];
                                                        //   vector<int> d(3);
                                                        //   int f = d[7];
                                                        // //   cout << f << endl;
                                                        //   int *g = nullptr;
                                                        //   *g = 1;

                                                          return;
                                                      },
                                                      "testTask", 9);
    auto mFuture1 = ThreadPool::GetInstance()->PostJob([](int num) -> int
                                                       {
                                                           ZYBK_TRACE();
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
