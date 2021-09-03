#ifndef _ZYBKLOG_H_
#define _ZYBKLOG_H_

#ifndef LOGENABLE
#define LOGENABLE 1
#endif
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stack>
#include <string.h>
#include <string>

using namespace std;

class __declspec(dllexport) LogManager;
class __declspec(dllexport) TraceManager;
class __declspec(dllexport) TraceWrapper;

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

#define ZYBK_TRACE(...)                                                                                                             \
    char *message = new char[1000];                                                                                                 \
    int i = 0;                                                                                                                      \
    *message = '\0';                                                                                                                \
    i += sprintf(message + i, "%-12s%-10s%-20s:%-20s:%-10d:  ", __DATE__, __TIME__, RMPATHr(RMPATH(__FILE__)), __func__, __LINE__); \
    TraceWrapper zybkTraceHELPPER(message);                                                                                         \
    do                                                                                                                              \
    {                                                                                                                               \
        delete[] message;                                                                                                           \
    } while (0)

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

class __declspec(dllexport) TraceManager
{
    stack<string> mTraceStack;

protected:
    TraceManager() = default;
    TraceManager(const TraceManager &) = delete;
    TraceManager operator=(const TraceManager &) = delete;

public:
    static TraceManager *GetInstance()
    {
        static TraceManager *mTraceManager = new TraceManager();
        return mTraceManager;
    }
    void addTrace(string &Trace)
    {
        mTraceStack.push(Trace);
    }
    void addTrace(string &&Trace)
    {
        mTraceStack.push(Trace);
    }
    void removeTrace()
    {
        mTraceStack.pop();
    }
    void dumpTrace()
    {
        std::fstream fs;
        fs.open("C:\\Users\\zy113\\vsProjects\\test.txt", std::fstream::in | std::fstream::out | std::fstream::app);

        while (!mTraceStack.empty())
        {
            string trace = mTraceStack.top();
            mTraceStack.pop();
            fs << trace << endl;
        }
        fs.close();
    }
};

class __declspec(dllexport) TraceWrapper
{
public:
    TraceWrapper(string &&trace)
    {
        TraceManager::GetInstance()->addTrace(trace);
        // TraceManager::GetInstance()->addTrace("zsfwafs");
    }
    TraceWrapper(string &trace)
    {
        TraceManager::GetInstance()->addTrace(trace);
        // TraceManager::GetInstance()->addTrace("1");
    }
    ~TraceWrapper()
    {
        TraceManager::GetInstance()->removeTrace();
    }
    TraceWrapper(const TraceWrapper &) = delete;
    TraceWrapper operator=(const TraceWrapper &) = delete;
};

#endif
