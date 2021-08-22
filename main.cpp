#include "ThreadPool.h"
#include "isValidBST.h"
#include "kmp.h"
#include "solutionEntry.h"
#include "test.h"
#include <iostream>
#include <thread>



int main(int, char **)
{
    Derive D;
    Base B;
    // D.func1();
    // D.func2();
    // D.Base::func1();
    D.Base::func2();
    // D.func2();
    B.func2();
    Derive *pD = new Derive();
    pD->func1();
    // pD->Base::func1();
    Base *pDb = &D;
    auto funn = [](Base *b)
    {
        b->func2();
        b->func1();
    };
    funn(pD);
    funn(pDb);

    std::thread t(fun);
    std::cout << "Hello, world!\n";
    SolutionIsValidBST().isValidBST(new TreeNode(0));
    string str1 = string("zybkisfun !!!");
    string str2 = string("isfun !");
    SolutionKMP().kmp(str1,str2);
    fun();
    t.join();
    //auto mFuture1=ThreadPool(10).enqueue(fun);
    //auto mFuture=ThreadPool(10).enqueue(std::move(&fun));
    auto mFuture = ThreadPool::GetInstance()->PostJob([]()
                                                      {
                                                          LOGD("try");
                                                          return;
                                                      });
    auto mFuture1 = ThreadPool::GetInstance()->PostJob([](int num)->int
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

        std::cout << "args threadTask = " << mFuture1.get() << std::endl;
    }
// 
    return 0;
}
