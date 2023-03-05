#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include "zybkLog.h"
#include "zybkTrace.h"
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

enum threadStatus : int
{
    THREAD_SUBMIT,
    THREAD_STOPED,
    THREAD_ERROR_ID,
};

enum threadType : int
{
    THREAD_TYPE_TASK,
    THREAD_TYPE_LOG,
    THREAD_TYPE_TRACE,
    THREAD_TYPE_MAX,
};

string threadStatus2str(threadStatus status);

struct ThreadTask
{
    threadType type;
    string taskName;
    std::function<void()> func;
};

class ThreadPool;

#define MAX_THREADS 8

class ThreadPool
{
public:
    int getNumThreads()
    {
        return mNumThreads;
    }
    /**
     *  @brief  deconstructor to clean up threads.
     *  when all threads finished, mostly they are in wait() state
     *  call this to notify them.
     */
    ~ThreadPool()
    {
        // {
        //     auto lock = std::unique_lock<std::mutex>(mThreadLock);
        //     isRunning = false;
        // }
        // for (int i = 0; i < THREAD_TYPE_MAX; i++)
        // {
        //     mCond[i].notify_all();
        // }
        // offline.get_future().get();
        // for (int i = 0; i < 10; i++)
        // {
        //     mThreads[i].detach();
        // }
    }
    /**
     *  @brief  tell ThreadPool current process is going down.
     *  call future.get() to block main function until all tasks finished.
     *  take care to stop loop tasks before you call this function
     *  otherwise main process will block,in that case we recommend
     *  you exit main without calling this function
     */
    auto checkOut()
    {
        // ZYBK_TRACE();
        // {
        //     auto lock = std::unique_lock<std::mutex>(mThreadLock);
        //     int mask = (0x1 << (THREAD_TYPE_TASK + 1)) - 1;
        //     isCheckingOut = true;
        //     if ((jobRemine & mask) == 0)
        //     {
        //         busy.set_value(false);
        //     }
        // }
        {
            auto lock = std::unique_lock<std::mutex>(mThreadLock);
            isRunning = false;
        }
        for (int i = 0; i < THREAD_TYPE_MAX; i++)
        {
            mCond[i].notify_all();
        }
        offline.get_future().get();
        for (int i = 0; i < 10; i++)
        {
            mThreads[i].join();
        }
        // return busy.get_future();
    }
    static ThreadPool *GetInstance();

    /**
     *  @brief  Post a task to ThreadPool.
     *  @param  task  a packaged task
     *  This function will %post the %func to the specified
     *  thread if assigned. threadPool related functions like LOGD&ZYBKTRACE,
     *  should not be presented in this function to avoid recursive call.
     */
    auto PostJob(ThreadTask &task) -> threadStatus
    {
        auto threadType = task.type;
        auto lock = std::unique_lock<std::mutex>(mThreadLock);
        if (!isRunning)
        {
            return THREAD_STOPED;
        }
        if (isCheckingOut && threadType == THREAD_TYPE_TASK)
            return THREAD_STOPED;
        if (threadType < THREAD_TYPE_TASK || threadType >= THREAD_TYPE_MAX)
            return THREAD_ERROR_ID;
        mTaskQueue[threadType].push(task);
        jobRemine |= 0x1 << threadType;
        mCond[threadType].notify_all();
        return THREAD_SUBMIT;
    }
    /**
     *  @brief  Post a task to ThreadPool.
     *  @param  func  a function pointer to be called,and due to current implementation,this function must not contain rvalue refrence imput params.
     *  @param  taskName  a string to identify task
     *  @param  threadId  should be -1 by default,set this Id to dispatch task to certain thread
     *  @param  args...  func input params
     *  This function will %post the %func to the specified
     *  thread if assigned.  If the %threadId is bigger than the
     *  threadPool's current thread num, the %threadId is set to 0.
     */
    // template <typename F, class... Args>
    // auto PostJob(F &&func, string taskName, int threadId, Args &&...args) -> std::future<decltype(func(args...))>
    // {

    //     //using return_type=typename std::result_of<F(Args...)>::type;
    //     // using mtype = typename std::result_of<F&(Args &&...)>::type;
    //     // using mtype1= decltype(forward<F>(func)(forward<Args>(args)...));
    //     // auto lock = std::unique_lock<std::mutex>(mThreadLock);
    //     // auto mbind=bind(std::forward<F>(func), std::forward<Args>(args)...);
    //     // auto mTask = std::make_shared<std::packaged_task<mtype1()>>(ref(mbind));
    //     using mtype = typename std::result_of<F&(Args &&...)>::type;
    //     // using mtype1= decltype(forward<F>(func)(forward<Args>(args)...));

    //     auto mTask = std::make_shared<std::packaged_task<mtype()>>(bind(std::forward<F>(func), std::forward<Args>(args)...));

    //     auto ret = mTask->get_future();
    //     threadId = threadId < 0 ? 0 : threadId;
    //     {
    //         auto lock = std::unique_lock<std::mutex>(mThreadLock);
    //     mTaskQueue[threadId].push({threadId >= mNumThreads ? 0 : threadId, taskName, {[mTask]()
    //                                                                                   { (*mTask)(); }}});
    //     jobRemine |= 0x1 << threadId;
    //     mCond[threadId].notify_all();

    //     }
    //     // LOGD("Posted!");
    //     return ret;
    // }
    // template <typename F>
    // auto PostJob(F&& func) -> std::future<typename std::result_of<F()>::type>
    // {
    //     using return_type = typename std::result_of<F()>::type;
    //     auto lock = std::unique_lock<std::mutex>(mThreadLock);
    //     auto mTask = std::make_shared<std::packaged_task<return_type()>>(func);
    //     mTaskQueue.push({[mTask]()
    //                      { (*mTask)(); }});
    //     mCond.notify_one();
    //     return mTask->get_future();
    // }

protected:
    ThreadPool(int num)
    {
        LOG_DEBUG("spawn threadNum %d %d", num, (unsigned int)GetCurrentThreadId());
        // ic.active();
        auto lock = std::unique_lock<std::mutex>(mThreadLock);
        int threadCfg[THREAD_TYPE_MAX] = {MAX_THREADS, 1, 1};
        mNumThreads = 0;
        isRunning = true;
        isCheckingOut = false;
        mTaskQueue.resize(THREAD_TYPE_MAX);
        jobRemine = 0;
        // mTaskQueue.
        int nnmu = 0;
        for (int i = 0; i < THREAD_TYPE_MAX; i++)
        {
            for (int j = 0; j < threadCfg[i]; j++)
            {
                mThreads.emplace_back(std::thread(threadLoop, this, (threadType)i));
                if (mThreads[nnmu++].joinable())
                    mNumThreads++;
            }
        }
        LOG_DEBUG("spawn joinable threadNum %d %d", mNumThreads, (unsigned int)GetCurrentThreadId());
    }
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool operator=(const ThreadPool &) = delete;

    void threadLoop(threadType type)
    {
        {
            LOG_DEBUG("threadId_%d %d num %d", type, (unsigned int)GetCurrentThreadId(), mNumThreads);
        }
        while (1)
        {
            ThreadTask task;
            {
                auto lock = std::unique_lock<std::mutex>(mThreadLock);
                mCond[type].wait(lock,
                                 [&]
                                 { return !isRunning || !mTaskQueue[type].empty(); });
                if (!isRunning && mTaskQueue[type].empty())
                    break;

                task = mTaskQueue[type].front();
                mTaskQueue[type].pop();
            }

            task.func();

            // {
            //     auto lock = std::unique_lock<std::mutex>(mThreadLock);
            //     LOG_DEBUG("threadPool2  !");
            //     if (!mTaskQueue[type].empty())
            //     {
            //         jobRemine |= 0x1 << type;
            //     }
            //     else
            //     {
            //         jobRemine &= ~(0x1 << type);
            //         if (isCheckingOut && ((jobRemine & 0xFF) == 0) && type == THREAD_TYPE_TASK)
            //         {
            //             busy.set_value(false);
            //             break;
            //         }
            //     }
            // }
        }
        {
            auto lock = std::unique_lock<std::mutex>(mThreadLock);
            mNumThreads--;
            LOG_DEBUG("exittt threadType_%d %d num %d", type, (unsigned int)GetCurrentThreadId(), mNumThreads);
            if (mNumThreads == 0)
            {
                offline.set_value(true);
            }
        }
    }

private:
    std::vector<std::thread> mThreads;
    std::vector<std::queue<ThreadTask>> mTaskQueue;
    std::condition_variable mCond[THREAD_TYPE_MAX];
    std::mutex mThreadLock;
    bool isRunning;
    bool isCheckingOut;
    int jobRemine;
    int mNumThreads;
    int threadCfg[THREAD_TYPE_MAX] = {MAX_THREADS, 1, 1};
    promise<bool> busy;
    promise<bool> offline;
};

class ThreadPoolManager
{
public:
    template <typename F, class... Args>
    static auto PostJob(F &&func, string taskName, threadType type, Args &&...args) -> pair<std::shared_future<decltype(func(args...))>, threadStatus>
    {
        // using mtype = typename std::result_of<F &(Args && ...)>::type;
        using mtype = decltype(func(args...));

        auto mTask = std::make_shared<std::packaged_task<mtype()>>(bind(std::forward<F>(func), std::forward<Args>(args)...));

        auto ret = shared_future<decltype(func(args...))>(mTask->get_future());
        ThreadTask task = {type, taskName, {[mTask]()
                                            { (*mTask)(); }}};
        threadStatus status = ThreadPool::GetInstance()->PostJob(task);
        LOG_DEBUG("task:%s ,threadid %d post statu:%s", taskName.c_str(), type, threadStatus2str(status).c_str());
        return pair<std::shared_future<decltype(func(args...))>, threadStatus>(ret, status);
    }
    static auto CheckOut()
    {
        return ThreadPool::GetInstance()->checkOut();
    }
};

#endif

// #ifndef THREAD_POOL_H
// #define THREAD_POOL_H

// #include <vector>
// #include <queue>
// #include <memory>
// #include <thread>
// #include <mutex>
// #include <condition_variable>
// #include <future>
// #include <functional>
// #include <stdexcept>

// class ThreadPool {
// public:
//     ThreadPool(size_t);
//     template<class F, class... Args>
//     auto enqueue(F&& f, Args&&... args)
//         -> std::future<typename std::result_of<F(Args...)>::type>;
//     ~ThreadPool();
// private:
//     // need to keep track of threads so we can join them
//     std::vector< std::thread > workers;
//     // the task queue
//     std::queue< std::function<void()> > tasks;

//     // synchronization
//     std::mutex queue_mutex;
//     std::condition_variable condition;
//     bool stop;
// };

// // the constructor just launches some amount of workers
// inline ThreadPool::ThreadPool(size_t threads)
//     :   stop(false)
// {
//     for(size_t i = 0;i<threads;++i)
//         workers.emplace_back(
//             [this]
//             {
//                 for(;;)
//                 {
//                     std::function<void()> task;

//                     {
//                         std::unique_lock<std::mutex> lock(this->queue_mutex);
//                         this->condition.wait(lock,
//                             [this]{ return this->stop || !this->tasks.empty(); });
//                         if(this->stop && this->tasks.empty())
//                             return;
//                         task = std::move(this->tasks.front());
//                         this->tasks.pop();
//                     }

//                     task();
//                 }
//             }
//         );
// }

// // add new work item to the pool
// template<class F, class... Args>
// auto ThreadPool::enqueue(F&& f, Args&&... args)
//     -> std::future<typename std::result_of<F(Args...)>::type>
// {
//     using return_type = typename std::result_of<F(Args...)>::type;

//     auto task = std::make_shared< std::packaged_task<return_type()> >(
//             std::bind(std::forward<F>(f), std::forward<Args>(args)...)
//         );

//     std::future<return_type> res = task->get_future();
//     {
//         std::unique_lock<std::mutex> lock(queue_mutex);

//         // don't allow enqueueing after stopping the pool
//         if(stop)
//             throw std::runtime_error("enqueue on stopped ThreadPool");

//         tasks.emplace([task](){ (*task)(); });
//     }
//     condition.notify_one();
//     return res;
// }

// // the destructor joins all threads
// inline ThreadPool::~ThreadPool()
// {
//     {
//         std::unique_lock<std::mutex> lock(queue_mutex);
//         stop = true;
//     }
//     condition.notify_all();
//     for(std::thread &worker: workers)
//         worker.join();
// }

// #endif