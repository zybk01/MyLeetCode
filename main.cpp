#include <iostream>
#include "solutionEntry.h"
#include "isValidBST.cpp"
#include<thread>
#include"test.h"
int main(int, char**) {
    std::thread t(fun);
    std::cout << "Hello, world!\n";
    Solution().isValidBST(new TreeNode(0));
    fun();
    t.join();
}
