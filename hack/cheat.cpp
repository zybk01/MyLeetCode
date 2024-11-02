#include "ThreadPool.h"
// #include "solutionEntry.h"
#include "zybkLog.h"
#include "zybkTrace.h"
#include "zybkopengl.h"
#include <Psapi.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <tlhelp32.h>
#include <windows.h>


#include "ThreadPool.h"
// #include "solutionEntry.h"
#include "zybkLog.h"
#include "zybkTrace.h"
#include "zybkopengl.h"
#include <Psapi.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <tlhelp32.h>
#include <windows.h>

int main(int num, char **args)
{
    ZYBK_TRACE();
    char input[256];
    if (num == 1)
    {
        LOGD("intput -i value to search");
    }
    int ketvalue = 99999;
    if (args)
    {
        for (int i = 0; i < num; i++)
        {
            if (args[i])
            {
                if (strcmp(args[i], "-i") == 0)
                {
                    snprintf(input, sizeof(input), "%s", args[i + 1]);
                    // rImage = string(input);
                    ketvalue = atoi(input);
                }
            }
        }
    }
    DWORD pid;
    int keyValue;
    // 1.找到游戏窗口 窗口类型、窗口标题
    HWND hwnd = FindWindow(NULL, "PlantsVsZombiesRH");
    // 2.通过窗口找到进程ID
    GetWindowThreadProcessId(hwnd, &pid);
    LOGD("PlantsVsZombiesRH pid %d", pid);
    // 3.通过进程ip打开进程
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    HANDLE snapShotHanlde = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, (DWORD)pid);
    map<ULONG_PTR, SIZE_T> processHeapMap;
    map<ULONG_PTR, SIZE_T> processFreeHeapMap;
    LPCVOID addr = 0;
    MEMORY_BASIC_INFORMATION mbi;
    int totalMem = 0;
    while (VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)))
    {
        DWORD64 addrd = (DWORD64)addr;
        if (mbi.State == MEM_COMMIT)
        {
            if (mbi.Type == MEM_PRIVATE && ((mbi.Protect | PAGE_EXECUTE_READWRITE) || (mbi.Protect | PAGE_READWRITE)))
            {
                LOGD("VirtualQueryEx allocation base: %p regionbase: %p  reguion size:%d mbi.Type %d", mbi.AllocationBase, addr, mbi.RegionSize, mbi.Type);
                processHeapMap.insert({addrd, mbi.RegionSize});
                totalMem += mbi.RegionSize;
            }
        }
        else if (mbi.State == MEM_FREE) {
            processFreeHeapMap.insert({addrd, mbi.RegionSize});
            LOGD("VirtualQueryEx Free allocation base: %p regionbase: %p  reguion size:%d mbi.Type %d", mbi.AllocationBase, addr, mbi.RegionSize, mbi.Type);
        }
        addrd += mbi.RegionSize;
        addr = (LPCVOID)addrd;
    }
    int neumrateNum = 0;
    map<ULONG_PTR, ULONG_PTR> matchMap;
    stack<int> matchStack;
    // write sun counts, byte pattern
    byte targetArray[] = {0x41, 0x2B, 0xC8 ,0x89, 0x88, 0x98, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x46, 0x48, 0x66, 0x0F, 0x57, 0xC0, 0xF2, 0x0F};
    // 41 2B C8 89 88 98 00 00 00 48 8B 46 48 66 0F 57 C0 F2 0F v2.0.4
    // byte targetArray[] = {0x41, 0x2B, 0xC8 ,0x89, 0x88, 0xA0, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x46, 0x40, 0x66, 0x0F, 0x57, 0xC0, 0xF2, 0x0F};
    // 41 2B C8 89 88 A0 00 00 00 48 8B 46 40 66 0F 57 C0 F2 0F V2.0
    // byte targetArray[] = {0x41, 0x2B, 0xC8 ,0x89, 0x88, 0x88, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x46, 0x40, 0x66, 0x0F, 0x57, 0xC0, 0xF2, 0x0F};
    // 41 2B C8 89 88 88 00 00 00 48 8B 46 40 66 0F 57 V1.3

    while (1) {
        // matchStack.push(ketvalue);
        map<ULONG_PTR, ULONG_PTR> curmatchMap;
        // if (matchStack.size() == 1) 
        {
            for (auto &itr : processHeapMap)
            {
                neumrateNum++;
                vector<byte> result(itr.second);
                SIZE_T resultsize = 0;
                // LOGD("totalMem %d region num %d/%d", totalMem / 1024 / 1024, neumrateNum, processHeapMap.size());
                ReadProcessMemory(hProcess, (LPCVOID)(itr.first), &result[0], itr.second, &resultsize);
                if (resultsize == itr.second) {
                    for (int i = 0; i < itr.second; i++)
                    {
                        bool comp = true;
                        for (int j = 0; j < sizeof(targetArray); j ++) {
                            if (targetArray[j] != result[i + j]) {
                                comp = false;
                                break;
                            }
                        }
                            if (comp)
                            // if (result[i] == ketvalue)
                            {
                                if (matchStack.size() == 1 && matchMap.count(itr.first + i))
                                {
                                    curmatchMap.insert({itr.first + i, itr.first});
                                }
                                else
                                {
                                    curmatchMap.insert({itr.first + i, itr.first});
                                }
                            }
                    }
                }
            }
        }
        LOGD("find curmatchMap %d matchMap %d", curmatchMap.size(), matchMap.size());
        matchMap = curmatchMap;
        if (matchMap.size() >= 1) {
            LOGD("find ketvalue %d addr %p, region addr %p: size %d", ketvalue, matchMap.begin()->first, matchMap.begin()->second, processHeapMap[matchMap.begin()->second]);
            break;
        }
        Sleep(500);
    };
    int result[16];
    char* transPtr;
    SIZE_T resultsize = 0;
    LPVOID newMem = VirtualAllocEx(hProcess, (LPVOID)(matchMap.begin()->first + 102400000), 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    while(newMem == nullptr) {
        newMem = VirtualAllocEx(hProcess, nullptr, 4096*16, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (((ULONG_PTR)newMem - matchMap.begin()->first) < 0xFFFFFFFF) {
            break;
        }
        LOGD("injecting code to addr %p failed, retry", newMem);
        VirtualFreeEx(hProcess, newMem, 4096*16, MEM_FREE);
        newMem = nullptr;
    }

    LOGD("injecting code to addr %p", newMem);
    if (newMem == nullptr)
        exit(0);
    // calculate offset from Injected addr to origin code
    int offsetCode = (char*)matchMap.begin()->first + 9 + 0x11 /* pass collling*/ - ((char*)newMem + 9 + 5);
    LOGD("offsetCode %x", offsetCode);
    byte InJectCode[] = {0x44, 0x01, 0xC1, 0x89, 0x88, 0x98, 0x00, 0x00, 0x00, 0xE9, (byte)(offsetCode & 0xFF), (byte)((offsetCode >> 8) & 0xFF), (byte)((offsetCode >> 16) & 0xFF), (byte)((offsetCode >> 24) & 0xFF)};
    // byte InJectCode[] = {0x44, 0x01, 0xC1, 0x89, 0x88, 0xA0, 0x00, 0x00, 0x00, 0xE9, (byte)(offsetCode & 0xFF), (byte)((offsetCode >> 8) & 0xFF), (byte)((offsetCode >> 16) & 0xFF), (byte)((offsetCode >> 24) & 0xFF)};
    // byte InJectCode[] = {0x44, 0x01, 0xC1, 0x89, 0x88, 0x88, 0x00, 0x00, 0x00, 0xE9, (byte)(offsetCode & 0xFF), (byte)((offsetCode >> 8) & 0xFF), (byte)((offsetCode >> 16) & 0xFF), (byte)((offsetCode >> 24) & 0xFF)};
    // calculate offset from origin addr to injected code
    int offset = (char*)newMem - ((char*)matchMap.begin()->first + 5);
    WriteProcessMemory(hProcess, newMem, InJectCode, sizeof(InJectCode), &resultsize);
    byte InJectJmp[] = {0xE9, (byte)(offset & 0xFF), (byte)((offset >> 8) & 0xFF), (byte)((offset >> 16) & 0xFF), (byte)((offset >> 24) & 0xFF), 0x0F , 0x1F,0x40,0x00};
    vector<byte> originCode(sizeof(InJectJmp));
    ReadProcessMemory(hProcess, (LPVOID)matchMap.begin()->first, &originCode[0], sizeof(InJectCode), &resultsize);
    {
        WriteProcessMemory(hProcess, (LPVOID)matchMap.begin()->first, InJectJmp, sizeof(InJectJmp), &resultsize);
    }
    while (ketvalue != 1) {
        string input;
        LOGD("type 1 value to restore :");
        getline(cin, input);
        int prekeyValue = matchStack.size() ? matchStack.top(): -1;
        ketvalue = atoi(input.c_str());
    }
    WriteProcessMemory(hProcess, (LPVOID)matchMap.begin()->first, &originCode[0], sizeof(InJectJmp), &resultsize);
    VirtualFreeEx(hProcess, newMem, 4096*16, MEM_FREE);
    CloseHandle(snapShotHanlde);
    CloseHandle(hProcess);
    return 0;
}
