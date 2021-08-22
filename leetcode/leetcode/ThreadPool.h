#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include "zybkLog.h"
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#define MAX_THREADS 15

struct ThreadTask
{

    std::function<void()> func;
};

class ThreadPool
{
public:
    ~ThreadPool()
    {
        LOGD(__func__);
        // std::cout << __FILE__ << ": " << __func__ << std::endl;
        {
            auto lock = std::unique_lock<std::mutex>(mThreadLock);
            isRunning = false;
        }
        mCond.notify_all();

        for (int i = 0; i < mNumThreads; i++)
        {
            mThreads[i].join();
        }
    }
    static ThreadPool *GetInstance()
    {
        static ThreadPool *mThreadPool = new ThreadPool(MAX_THREADS);
        return mThreadPool;
    }
    template <typename F, class... Args>
    auto PostJob(F &&func, Args &&...args) -> std::future<decltype(func(args...))>
    {
        //using return_type=typename std::result_of<F(Args...)>::type;
        using mtype = decltype(func(args...));
        auto lock = std::unique_lock<std::mutex>(mThreadLock);
        
        auto mTask = std::make_shared<std::packaged_task<mtype()>>(std::bind(std::forward<F>(func), std::forward<Args>(args)...));
        auto ret=mTask->get_future();
        
        mTaskQueue.push({[mTask]()
                         { (*mTask)(); }});
        mCond.notify_one();
        // LOGD("Posted!");
        return ret;
    }
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
        LOGD("Starting Threads, Threads number =%d", num);
        // std::cout << __FILE__ << ": " << __func__ << " Threads number = " << num << std::endl;
        mNumThreads = num;
        isRunning = true;
        //mTaskQueue.
        for (int i = 0; i < num; i++)
        {
            mThreads.push_back(std::thread(threadLoop, this));
        }
    }
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool operator=(const ThreadPool &) = delete;

    void threadLoop()
    {
        while (isRunning)
        {
            ThreadTask task;
            bool valid = false;
            {
                auto lock = std::unique_lock<std::mutex>(mThreadLock);
                if (isRunning && mTaskQueue.empty())
                {
                    mCond.wait(lock);
                }
                if (!mTaskQueue.empty())
                {
                    task = mTaskQueue.front();
                    mTaskQueue.pop();
                    valid = true;
                }
            }
            if (isRunning&&valid)
            {
                try{
                    task.func();
                }catch(exception e){
                    LOGD(e.what());
                }
                
                // LOGD("working!");
            }
        }
    }

private:
    std::vector<std::thread> mThreads;
    std::queue<ThreadTask> mTaskQueue;
    std::mutex mThreadLock;
    std::condition_variable mCond;
    bool isRunning;
    int mNumThreads;
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