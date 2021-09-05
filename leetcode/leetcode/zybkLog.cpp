#include "zybkLog.h"
#include "ThreadPool.h"
// #include "zybkTrace.h"

LogManager *LogManager::GetInstance()
{
    // cout << "logManager\n";
    static LogManager *mInstance = new LogManager();
    return mInstance;
}

void LogManager::postMessage(string message)
{
    ThreadPoolManager::PostJob([](string str)
                                       { cout << str << endl; },
                                       "LogManager", 9, message).get();
}