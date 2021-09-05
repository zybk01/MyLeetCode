#include "ThreadPool.h"

ThreadPool *ThreadPool::GetInstance()
{
    // ZYBK_TRACE(); //should not present here as well as LOGD,for they internally require this function to be called.
    static ThreadPool *mThreadPool = new ThreadPool(MAX_THREADS);
    return mThreadPool;
}