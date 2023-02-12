#include "zybkLog.h"
#include "ThreadPool.h"

LogManager *LogManager::GetInstance()
{
    static LogManager *mInstance = new LogManager();
    return mInstance;
}

void LogManager::postMessage(string message)
{
    LOG_DEBUG(message.c_str());
    auto postStatus = ThreadPoolManager::PostJob([](string str)
                                       { cout << str << endl; },
                                       "LogManager", THREAD_TYPE_LOG, message);
    if (postStatus.second == threadStatus::THREAD_SUBMIT)
        postStatus.first.get();
}