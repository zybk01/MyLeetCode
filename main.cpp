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
#include "zybkopengl.h"


void TraceCheck()
{
    ZYBK_TRACE();
    int *g = nullptr;
    *g = 1;
}
int main(int num, char ** args)
{
    ZYBK_TRACE();
    for (int i = 0; i < num; i++) {
        // LOGD("main input [%d] : %s", i, args[i]);
    }
    SolutionIsValidBST().isValidBST(new TreeNode(0));
    opengl();
    auto checkoutsignal = ThreadPool::GetInstance()->checkOut();
    checkoutsignal.get();
    // system("pause");
    return 0;
}
