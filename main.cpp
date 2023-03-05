#include "ThreadPool.h"
// #include "solutionEntry.h"
#include "zybkLog.h"
#include "zybkTrace.h"
#include "zybkopengl.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <windows.h>
// typedef void *(*getThreadPool)();
// HMODULE libHandle = LoadLibrary("libzybk.util.dll");
// if (libHandle == NULL)
// {
//     return -1;
// }
// getThreadPool pPool = reinterpret_cast<getThreadPool>(GetProcAddress(libHandle, "getThreadPool"));
// if (pPool == NULL)
// {
//     return -1;
// }
// // system("pause");
// pPool();
// FreeLibrary(libHandle);


int main(int num, char ** args)
{
    ZYBK_TRACE();
    // int *ptr;
    // *ptr = 000;
    // SolutionIsValidBST().isValidBST(new TreeNode(0));
    opengl();

    ThreadPoolManager::CheckOut();

    // system("pause");

    return 0;
}