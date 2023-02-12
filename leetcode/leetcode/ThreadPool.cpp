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


class OOO{
    public:
    OOO(){
        LOG_DEBUG("cons spawn threadNum %d", (unsigned int)GetCurrentThreadId());
    }
    void aa(){
        LOG_DEBUG("spawn threadNum %d", (unsigned int)GetCurrentThreadId());
    }
    ~OOO()
    {
        LOG_DEBUG("decons spawn threadNum %d", (unsigned int)GetCurrentThreadId());
    }
};
static OOO ooo;
// EXTERN_C {
//     void* __declspec(dllexport) getThreadPool() {
//         return ThreadPool::GetInstance();
//     }
// }
ThreadPool* __declspec(dllexport) ThreadPool::GetInstance()
{
    // ZYBK_TRACE(); //should not present here as well as LOGD,for they internally require this function to be called.
    // static ThreadPool mThreadPool(MAX_THREADS);
    // return &mThreadPool;
        ooo.aa();
    static ThreadPool mThreadPool(MAX_THREADS);
    return &mThreadPool;
}