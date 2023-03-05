#ifndef _ZYBKLOG_H_
#define _ZYBKLOG_H_

#ifndef LOGENABLE
#define LOGENABLE 1
#endif
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string.h>
#include <string>
using namespace std;

class __declspec(dllexport) LogManager;

#define RMPATH(x) (strrchr(x, '\\') ? strrchr(x, '\\') : x)
#define RMPATHr(x) (strrchr(x, '/') ? strrchr(x, '/') : x)
#define LOGD(...)                                                                                                                                   \
    do                                                                                                                                              \
    {                                                                                                                                               \
        if (LOGENABLE)                                                                                                                              \
        {                                                                                                                                           \
            char *ZYBKLOG_message = new char[1000];                                                                                                 \
            int ZYBKLOG_i = 0;                                                                                                                      \
            *ZYBKLOG_message = '\0';                                                                                                                \
            SYSTEMTIME ZYBKLOG_time;                                                                                                                \
            GetLocalTime(&ZYBKLOG_time);                                                                                                            \
            time_t ZYBKLOG_time1 = time(nullptr);                                                                                                   \
            ZYBKLOG_i += sprintf(ZYBKLOG_message + ZYBKLOG_i, "%4d/%02d/%02d %02d:%02d:%02d.%03d ", ZYBKLOG_time.wYear,                             \
                                 ZYBKLOG_time.wMonth, ZYBKLOG_time.wDay, ZYBKLOG_time.wHour, ZYBKLOG_time.wMinute,                                  \
                                 ZYBKLOG_time.wSecond, ZYBKLOG_time.wMilliseconds);                                                                 \
            ZYBKLOG_i += sprintf(ZYBKLOG_message + ZYBKLOG_i, "%d %d:  ", (unsigned int)GetCurrentProcessId(), (unsigned int)GetCurrentThreadId()); \
            ZYBKLOG_i += sprintf(ZYBKLOG_message + ZYBKLOG_i, "%-20s:%-20s:LINE %d:  ", RMPATHr(RMPATH(__FILE__)), __func__, __LINE__);             \
            ZYBKLOG_i += sprintf(ZYBKLOG_message + ZYBKLOG_i, __VA_ARGS__);                                                                         \
            LogManager::GetInstance()->postMessage(ZYBKLOG_message);                                                                                \
            delete[] ZYBKLOG_message;                                                                                                               \
        }                                                                                                                                           \
    } while (0)

#define LOG_DEBUG(...)                                                                                                                              \
    do                                                                                                                                              \
    {                                                                                                                                               \
        if (DEBUG_ENABLE)                                                                                                                           \
        {                                                                                                                                           \
            char *ZYBKLOG_message = new char[1000];                                                                                                 \
            int ZYBKLOG_i = 0;                                                                                                                      \
            *ZYBKLOG_message = '\0';                                                                                                                \
            SYSTEMTIME ZYBKLOG_time;                                                                                                                \
            GetLocalTime(&ZYBKLOG_time);                                                                                                            \
            time_t ZYBKLOG_time1 = time(nullptr);                                                                                                   \
            ZYBKLOG_i += sprintf(ZYBKLOG_message + ZYBKLOG_i, "[DEBUG LOG]:%4d/%02d/%02d %02d:%02d:%02d.%03d ", ZYBKLOG_time.wYear,                 \
                                 ZYBKLOG_time.wMonth, ZYBKLOG_time.wDay, ZYBKLOG_time.wHour, ZYBKLOG_time.wMinute,                                  \
                                 ZYBKLOG_time.wSecond, ZYBKLOG_time.wMilliseconds);                                                                 \
            ZYBKLOG_i += sprintf(ZYBKLOG_message + ZYBKLOG_i, "%d %d:  ", (unsigned int)GetCurrentProcessId(), (unsigned int)GetCurrentThreadId()); \
            ZYBKLOG_i += sprintf(ZYBKLOG_message + ZYBKLOG_i, "%-20s:%-20s:LINE %d:  ", RMPATHr(RMPATH(__FILE__)), __func__, __LINE__);             \
            ZYBKLOG_i += sprintf(ZYBKLOG_message + ZYBKLOG_i, "[DEBUG MESSAGE]:");                                                                  \
            ZYBKLOG_i += sprintf(ZYBKLOG_message + ZYBKLOG_i, __VA_ARGS__);                                                                         \
            cout << ZYBKLOG_message << endl;                                                                                                        \
            {                                                                                                                                       \
                std::fstream fs;                                                                                                                    \
                char path[1000];                                                                                                                    \
                _getcwd(path, 1000);                                                                                                                \
                sprintf(path, "%s/log.txt", path);                                                                                                  \
                fs.open(path, std::fstream::in | std::fstream::out | std::fstream::app);                                                            \
                fs << ZYBKLOG_message << "\n"                                                                                                       \
                   << endl;                                                                                                                         \
                                                                                                                                                    \
                fs.close();                                                                                                                         \
            }                                                                                                                                       \
            delete[] ZYBKLOG_message;                                                                                                               \
        }                                                                                                                                           \
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
