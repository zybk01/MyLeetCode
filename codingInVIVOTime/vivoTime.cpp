#include "zybkLog.h"
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

using namespace std;

mutex mmlock;
condition_variable con[2];

class TryStatic
{
    static TryStatic *GTryStatic;
    class wrapper
    {
        ~wrapper()
        {
            delete TryStatic::GTryStatic;
        }
    };
    static wrapper mWrapper;
    // const static TryStatic* mTryStatic=new TryStatic();
    TryStatic()
    {
        cout << "construct TryStatic!!!" << endl;
    }

public:
    static TryStatic GGTryStatic;
    static TryStatic *GetInstancePtr()
    {
        // GTryStatic = new TryStatic();
        TryStatic *mGTryStatic = new TryStatic();
        // if (mGTryStatic == nullptr)
        // {
        //     mGTryStatic = new TryStatic();
        // }
        return mGTryStatic;
    }
    static TryStatic GetInstance();

    void print()
    {
        cout << "TryStatic Print!!" << endl;
    }
    ~TryStatic()
    {
        cout << "deconstruct TryStatic !!!" << endl;
    }
};

TryStatic *TryStatic::GTryStatic = new TryStatic();
TryStatic TryStatic::GGTryStatic;
TryStatic TryStatic::GetInstance()
{
    GGTryStatic.print();
    TryStatic mGTryStatic;
    // if (mGTryStatic == nullptr)
    // {
    //     mGTryStatic = new TryStatic();
    // }
    return mGTryStatic;
}
class A
{
public:
    A()
    {
        cout << "construct A id "<<this_thread::get_id() << endl;
    }
    A(const A &a)
    {
        cout << "copy construct A id " <<this_thread::get_id() << endl;
    }

    ~A()
    {
        cout << "~construct A id "<<this_thread::get_id()  << endl;
    }
};

bool flag = false;
void fun1(unique_ptr<string> &&ptr)
{
    // unique_lock<mutex> mlock(mmlock);
    // if (!flag)
    //     con[0].wait(mlock);
    // if (ptr.get() == nullptr)
    // {
    //     cout << "failed" << endl;
    // }
    cout << ptr.get()->c_str() << endl;
    con[1].notify_all();
}
void fun2(A &&ptr)
{
    cout << "in fun2" << endl;
    unique_lock<mutex> mlock(mmlock);
    // if (!flag)
    //     con[0].wait(mlock);
    cout << "fun2" << endl;
    // cout << ptr.get()->c_str() << endl;
    con[1].notify_all();
}
A fun3(A &ptr){
    return ptr;
}

void fun()
{

    unique_ptr<string> ptr(new string("aasdfgh"));
    A a;
    thread t1(fun2, move(a));
    ptr.release();
    // cout << ptr.get()->c_str() << endl;
    // t1.join();
    t1.detach();
    cout << "fun" << endl;
}

int main()
{
    cout << "Hello World!!" << endl;
    A a;
    move(a);
    // thread t1(fun2,move(a));
    fun2(move(a));
    fun3(a);
    unique_ptr<string> ptr(new string("aasdfgh"));
    fun1(move(ptr));
    cout << ptr.get()->c_str() << endl;
    // fun3(move(a));
    flag = true;
    // con[0].notify_all();
// t1.join();
    // TryStatic::GetInstancePtr()->print();
    // TryStatic::GetInstance().print();
    // TryStatic::GGTryStatic.print();

    system("pause");
    return 0;
}