#ifndef _KMP_H_
#define _KMP_H_

#include "solutionEntry.h"
#include "zybkLog.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

using namespace std;

// class TryStatic
// {
//     static TryStatic *GTryStatic;
//     class wrapper{
//         ~wrapper(){
//             delete TryStatic::GTryStatic;
//         }
//     };
//     static wrapper mWrapper;
//     // const static TryStatic* mTryStatic=new TryStatic();
//     TryStatic()
//     {
//         cout << "construct TryStatic!!!" << endl;
//     }

// public:
//     static TryStatic GGTryStatic;
//     static TryStatic *GetInstancePtr()
//     {
//         // GTryStatic = new TryStatic();
//         TryStatic *mGTryStatic = new TryStatic();
//         // if (mGTryStatic == nullptr)
//         // {
//         //     mGTryStatic = new TryStatic();
//         // }
//         return mGTryStatic;
//     }
//     static TryStatic GetInstance();

//     void print()
//     {
//         cout << "TryStatic Print!!" << endl;
//     }
//     ~TryStatic()
//     {
//         cout << "deconstruct TryStatic !!!" << endl;
//     }
// };

// TryStatic *TryStatic::GTryStatic = new TryStatic();
// TryStatic TryStatic::GGTryStatic;
// TryStatic TryStatic::GetInstance()
// {
//     GGTryStatic.print();
//     TryStatic mGTryStatic;
//     // if (mGTryStatic == nullptr)
//     // {
//     //     mGTryStatic = new TryStatic();
//     // }
//     return mGTryStatic;
// }
class __declspec(dllexport) SolutionKMP
{
public:
    int kmp(string &T, string &P);
    vector<int> computePrefix(string &P);
};

#endif