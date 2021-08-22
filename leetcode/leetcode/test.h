#include <iostream>

using namespace std;
void fun();

class Base
{
public:
    virtual void func1()
    {
        cout << "Base: func1" << endl;
    }

public:
    void func2()
    {
        cout << "Base: func2" << endl;
        func1();
    }
};

class Derive : public Base
{
    public:
    void func1()
    {
        cout << "Derive: func1" << endl;
    }
    public:
        using Base::func2;
        void func2() = delete;
};