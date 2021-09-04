#ifndef _ZYBKLOG_H_
#define _ZYBKLOG_H_

#ifndef LOGENABLE
#define LOGENABLE 1
#endif
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string.h>
#include <string>

using namespace std;

class __declspec(dllexport) LogManager;


#define RMPATH(x) (strrchr(x, '\\') ? strrchr(x, '\\') : x)
#define RMPATHr(x) (strrchr(x, '/') ? strrchr(x, '/') : x)
#define LOGD(...)                                                                                                           \
    do                                                                                                                      \
    {                                                                                                                       \
        if (LOGENABLE)                                                                                                      \
        {                                                                                                                   \
            char *message = new char[1000];                                                                                 \
            int i = 0;                                                                                                      \
            *message = '\0';                                                                                                \
            i += sprintf(message + i, "%-12s%-10s%-20s:%-20s:  ", __DATE__, __TIME__, RMPATHr(RMPATH(__FILE__)), __func__); \
            i += sprintf(message + i, __VA_ARGS__);                                                                         \
            LogManager::GetInstance()->postMessage(message);                                                                \
            delete[] message;                                                                                               \
        }                                                                                                                   \
    } while (0)

// i += sprintf(message + i, "\n");

// #define ZYBK_TRACE TraceWrapper zybkTraceHELPPER(string(__DATE__) + string(__TIME__) + string(RMPATHr(RMPATH(__FILE__))) + string(__func__) + to_string(__LINE__))



class __declspec(dllexport) LogManager
{
protected:
    LogManager() = default;
    LogManager(const LogManager &) = delete;
    LogManager operator=(const LogManager &) = delete;

public:
    static LogManager *GetInstance();
    void postMessage(string);
};


#endif
