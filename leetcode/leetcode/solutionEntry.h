#ifndef SOLUTION_ENTRY
#define SOLUTION_ENTRY

#include<iostream>
#include<string>

typedef void *pHandle;
typedef void **pData;

// struct solutionEntry;

template <typename... Args>
void (*Solve)(pHandle handle, Args&... args);

typedef void (*solvefunc)(pHandle, std::string&, std::string&, int&);

void sss(...){

    return;
}
struct solutionEntryBase
{
    /* data */
    void (*create)(pHandle &out);

    void* solve;

    void (*destroy)(pHandle handle);
    
};

struct solutionEntry:solutionEntryBase{
    template <class... Args>
    void process(pHandle handle,Args&... args)
    {

        std::cout << __FILE__<<": process" << std::endl;
        // solvefunc func2= reinterpret_cast<solvefunc>(solve);
        // func2(handle,args...);
        //solvefunc func1= reinterpret_cast<decltype(Solve<Args...>)>(solve);
        // func1(handle,args...);
        auto func=reinterpret_cast<void(*)(pHandle,Args&...)>(solve);
        func(handle, args...);
        //Solve<int>  = std::move([](pHandle,int &a){return ;});
       // Solve < std::string, std::string, int >= std::move(reinterpret_cast<void (*)(pHandle, std::string &, std::string &, int &)>(solve));
        //reinterpret_cast<void(*)(pHandle,Args&...)>(solve);
        // Solve<Args...>(handle,args...);
        std::cout<< __FILE__ << ": processed" << std::endl;
    }

};

typedef void (*EntryFunc)(solutionEntryBase *entry);

// struct solutionEntry;

// typedef void (*EntryFunc)(solutionEntry &entry);

// struct solutionEntry
// {
//     /* data */
//     void (*create)(pHandle& out);
//     void (*solve)(pHandle handle,pData dataptr,void *out);
//     void (*destroy)(pHandle handle);
// };

#endif