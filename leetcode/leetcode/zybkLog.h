#ifndef _ZYBKLOG_H_
#define _ZYBKLOG_H_

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <string.h>

using namespace std;
#define RMPATH(x) (strrchr(x, '\\') ? strrchr(x, '\\') : x)
#define LOGD(...)                                                                           \
    do                                                                                      \
    {                                                                                       \
        printf("%-12s%-10s%-20s:%-20s:  ", __DATE__, __TIME__, RMPATH(__FILE__), __func__); \
        printf(__VA_ARGS__);                                                              \
        printf("\n");                                                                       \
    } while (0)

#endif
