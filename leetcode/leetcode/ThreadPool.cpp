#include "ThreadPool.h"

string threadStatus2str(threadStatus status) {
    switch(status) {
        case THREAD_SUBMIT:
            return "THREAD_SUBMIT";
        case THREAD_STOPED:
            return "THREAD_STOPED";
        case THREAD_ERROR_ID:
            return "THREAD_ERROR_ID";
        default:
            return "THREAD_ERROR_UNKNOWN_STATUS";
    }
}

ThreadPool *ThreadPool::GetInstance()
{
    // ZYBK_TRACE(); //should not present here as well as LOGD,for they internally require this function to be called.
    static ThreadPool mThreadPool(MAX_THREADS);
    return &mThreadPool;
    // static ThreadPool *mThreadPool = new ThreadPool(MAX_THREADS);
    // return mThreadPool;
}