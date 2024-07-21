#include "ThreadPool.h"
// #include "solutionEntry.h"
#include "zybkLog.h"
#include "zybkTrace.h"
#include "zybkopengl.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <windows.h>
#include <thread>
// typedef void *(*getThreadPool)();
// HMODULE libHandle = LoadLibrary("libzybk.util.dll");
// if (libHandle == NULL)
// {
//     return -1;
// }
// getThreadPool pPool = reinterpret_cast<getThreadPool>(GetProcAddress(libHandle, "getThreadPool"));
// if (pPool == NULL)
// {
//     return -1;
// }
// // system("pause");
// pPool();
// FreeLibrary(libHandle);
class Base {
    public:
    Base() {
        LOGD("Base");
    }
    ~Base() {
        LOGD("~Base");
    }
    int a = 0;
};
class A
{
public:
    A()
    {
        ZYBK_TRACE();
        vec.resize(10);
        LOGD("test %p this %p", &vec[0], this);
    };
    virtual ~A()
    {
        ZYBK_TRACE();
        LOGD("test this %p", this);
    };
    virtual void aaa()
    {
        LOGD("test");
    }
    std::vector<int> vec;
    Base b;
};

class B
{
public:
    B()
    {
        ZYBK_TRACE();
        vec.resize(10);
        LOGD("test %p this %p", &vec[0], this);
    };
    virtual ~B()
    {
        ZYBK_TRACE();
        LOGD("test this %p", this);
    };
    virtual void bbb()
    {
        LOGD("test");
    }
    void bbbNV()
    {
        LOGD("test");
    }
    std::vector<int> vec;
    Base b;
};

class C : public A, public B
{
public:
    C()
    {
        ZYBK_TRACE();
        vec.resize(10);
        LOGD("test %p this %p", &vec[0], this);
    };
    virtual ~C()
    {
        ZYBK_TRACE();
        LOGD("test this %p", this);
    };
    virtual void bbb()
    {
        LOGD("test");
    }
    virtual void aaa()
    {
        LOGD("test");
    }
    std::vector<int> vec;
    Base b;
};

int main(int num, char **args)
{
    ZYBK_TRACE();
    // int *ptr;
    // *ptr = 000;
    // SolutionIsValidBST().isValidBST(new TreeNode(0));
    opengl();
    C *c = new C();
    c->aaa();
    c->bbb();
    LOGD("ptr %p %p", c, dynamic_cast<A *>(c));
    LOGD("ptr %p %p", c, dynamic_cast<B *>(c));
    delete c;
    LOGD("ptr %p %p", c, dynamic_cast<B *>(c));
    // delete c;
    A *a = new C();
    a->aaa();
    // a->bbb();
    LOGD("ptr %p %p", a, dynamic_cast<C *>(a));
    delete a;
    B *b = new C();
    // b->aaa();
    long long l = (long long)b;
    l = l >> 7 << 7;
    LOGD("ptr %p %p %p", b, dynamic_cast<C *>(b), (B *)l);
    B* bb = (B *)l;
    //  std::chrono::duration<;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // LOGD("ptr %p %p", b, dynamic_cast<C *>(bb));
    bb->bbbNV();
    LOGD("ptr %p %p", b, dynamic_cast<C *>(b));
    b->bbb();
    delete b;
    ThreadPoolManager::CheckOut();

    system("pause");

    return 0;
}