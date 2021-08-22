#ifndef _ZYBKLOG_H_
#define _ZYBKLOG_H_

#ifndef LOGENABLE 
#define LOGENABLE 1
#endif
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <string.h>

using namespace std;
#define RMPATH(x) (strrchr(x, '\\') ? strrchr(x, '\\') : x)
#define RMPATHr(x) (strrchr(x, '/') ? strrchr(x, '/') : x)
#define LOGD(...)                                                                               \
    do                                                                                          \
    {                                                                                           \
        if (LOGENABLE)                                                                          \
        {                                                                                       \
            printf("%-12s%-10s%-20s:%-20s:  ", __DATE__, __TIME__, RMPATHr(RMPATH(__FILE__)), __func__); \
            printf(__VA_ARGS__);                                                                \
            printf("\n");                                                                       \
        }                                                                                       \
    } while (0)

#endif
