#include "ThreadPool.h"
#include "isValidBST.h"
#include "kmp.h"
#include "solutionEntry.h"
#include "test.h"
#include "zybkLog.h"
#include "zybkTrace.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <windows.h>


void TraceCheck()
{
    ZYBK_TRACE();
    int *g = nullptr;
    *g = 1;
}
int main(int, char **)
{
    ZYBK_TRACE();
    // SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
    SolutionIsValidBST().isValidBST(new TreeNode(0));
    ThreadPoolManager::PostJob(TraceCheck, "wrong", 1).get();
    auto checkoutsignal = ThreadPool::GetInstance()->checkOut();
    checkoutsignal.get();
    // system("pause");
    return 0;
}
