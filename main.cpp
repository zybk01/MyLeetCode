#include "ThreadPool.h"
#include "isValidBST.cpp"
#include "solutionEntry.h"
#include "test.h"
#include <iostream>
#include <thread>
int main(int, char **)
{
    std::thread t(fun);
    std::cout << "Hello, world!\n";
    Solution().isValidBST(new TreeNode(0));
    fun();
    t.join();
    //auto mFuture1=ThreadPool(10).enqueue(fun);
    //auto mFuture=ThreadPool(10).enqueue(std::move(&fun));
    auto mFuture = ThreadPool::GetInstance()->PostJob([]()
                                                      { return 0; });
    auto mFuture1 = ThreadPool::GetInstance()->PostJob([](int num)
                                                      {
                                                         // std::cout << "args threadTask = " << num << std::endl;
                                                          return num;
                                                      },
                                                      10);
    if (mFuture.valid())
    {
        mFuture.get();
        std::cout << "no args threadTask" << std::endl;
    }
    if (mFuture1.valid())
    {
        
        std::cout << "args threadTask = " <<mFuture1.get()<< std::endl;
    }

    return 0;
}
