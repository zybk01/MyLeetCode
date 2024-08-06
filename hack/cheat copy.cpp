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
    // PROCESS_MEMORY_COUNTERS memoryCounter;
    // GetProcessMemoryInfo(hProcess, &memoryCounter, sizeof(memoryCounter));
    HANDLE snapShotHanlde = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, (DWORD)pid);
    // PHEAPENTRY32 heapEntry;
    // Heap32First(&heapEntry, (DWORD)hProcess);
    // HEAPLIST32 hl;
    // hl.dwSize = sizeof(HEAPLIST32);
    map<ULONG_PTR, SIZE_T> processHeapMap;
    // if (Heap32ListFirst(snapShotHanlde, &hl))
    // {
    //     do
    //     {
    //         HEAPENTRY32 he;
    //         ZeroMemory(&he, sizeof(HEAPENTRY32));
    //         he.dwSize = sizeof(HEAPENTRY32);

    //         if (Heap32First(&he, pid, hl.th32HeapID))
    //         {
    //             // LOGD("\nHeap ID: %lu\n", hl.th32HeapID);
    //             do
    //             {
    //                 // LOGD("Block addr:%p size: %lu\n", he.dwAddress, he.dwBlockSize);
    //                 processHeapMap.insert({he.dwAddress, he.dwBlockSize});
    //                 he.dwSize = sizeof(HEAPENTRY32);
    //             } while (Heap32Next(&he));
    //         }
    //         hl.dwSize = sizeof(HEAPLIST32);
    //     } while (Heap32ListNext(snapShotHanlde, &hl));

    // if (Heap32First(&he, pid, hl.th32HeapID))
    // {
    //     HANDLE hHeap = he.hHandle;
    //     PROCESS_HEAP_ENTRY Entry;
    //     if (HeapLock(hHeap) == FALSE)
    //     {
    //         LOGD(TEXT("Failed to lock heap with LastError %d.\n"),
    //                  GetLastError());
    //         return 1;
    //     }

    //     LOGD(TEXT("Walking heap %#p...\n\n"), hHeap);

    //     Entry.lpData = NULL;
    //     while (HeapWalk(hHeap, &Entry) != FALSE)
    //     {
    //         if ((Entry.wFlags & PROCESS_HEAP_ENTRY_BUSY) != 0)
    //         {
    //             LOGD(TEXT("Allocated block"));

    //             if ((Entry.wFlags & PROCESS_HEAP_ENTRY_MOVEABLE) != 0)
    //             {
    //                 LOGD(TEXT(", movable with HANDLE %#p"), Entry.Block.hMem);
    //             }

    //             if ((Entry.wFlags & PROCESS_HEAP_ENTRY_DDESHARE) != 0)
    //             {
    //                 LOGD(TEXT(", DDESHARE"));
    //             }
    //         }
    //         else if ((Entry.wFlags & PROCESS_HEAP_REGION) != 0)
    //         {
    //             LOGD(TEXT("Region\n  %d bytes committed\n")
    //                          TEXT("  %d bytes uncommitted\n  First block address: %#p\n")
    //                              TEXT("  Last block address: %#p\n"),
    //                      Entry.Region.dwCommittedSize,
    //                      Entry.Region.dwUnCommittedSize,
    //                      Entry.Region.lpFirstBlock,
    //                      Entry.Region.lpLastBlock);
    //         }
    //         else if ((Entry.wFlags & PROCESS_HEAP_UNCOMMITTED_RANGE) != 0)
    //         {
    //             LOGD(TEXT("Uncommitted range\n"));
    //         }
    //         else
    //         {
    //             LOGD(TEXT("Block\n"));
    //         }

    //         LOGD(TEXT("  Data portion begins at: %#p\n  Size: %d bytes\n")
    //                      TEXT("  Overhead: %d bytes\n  Region index: %d\n\n"),
    //                  Entry.lpData,
    //                  Entry.cbData,
    //                  Entry.cbOverhead,
    //                  Entry.iRegionIndex);
    //     }

    //     //
    //     // Unlock the heap to allow other threads to access the heap after
    //     // enumeration has completed.
    //     //
    //     if (HeapUnlock(hHeap) == FALSE)
    //     {
    //         LOGD(TEXT("Failed to unlock heap with LastError %d.\n"),
    //                  GetLastError());
    //     }
    // }
    // }
    // else
    //     LOGD("Cannot list first heap (%s)\n", getLastErrorFromCode().c_str());
    // LOGD("Cannot list first heap (%s)\n", getLastErrorFromCode().c_str());

    // for (auto &itr : processHeapMap)
    // {
    //     int result[itr.second];
    //     SIZE_T resultsize = 0;
    //     ReadProcessMemory(hProcess, (LPCVOID)(itr.first), &result, itr.second, &resultsize);
    //     for (int i = 0; i < itr.second / 4; i)
    //     {
    //         if (resultsize == 4 && result[i] == ketvalue)
    //         {
    //             LOGD("got addr %p", itr.first + 4 * i);
    //         }
    //     }
    // }
    // initializeProcessForWsWatch(hProcess);
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
        addrd += mbi.RegionSize;
        addr = (LPCVOID)addrd;
    }
    int neumrateNum = 0;
    map<ULONG_PTR, ULONG_PTR> matchMap;
    stack<int> matchStack;
    while(1) {
        string input;
        LOGD("type in value to search this time :");
        getline(cin, input);
        int prekeyValue = matchStack.size() ? matchStack.top(): -1;
        ketvalue = atoi(input.c_str());
        matchStack.push(ketvalue);
        map<ULONG_PTR, ULONG_PTR> curmatchMap;
        if (matchStack.size() == 1) {
            for (auto &itr : processHeapMap)
            {
                neumrateNum++;
                vector<int> result(itr.second / 4);
                SIZE_T resultsize = 0;
                // LOGD("totalMem %d region num %d/%d", totalMem / 1024 / 1024, neumrateNum, processHeapMap.size());
                ReadProcessMemory(hProcess, (LPCVOID)(itr.first), &result[0], itr.second, &resultsize);
                if (resultsize == itr.second) {
                    for (int i = 0; i < itr.second / 4; i++)
                    {
                        if (result[i] == ketvalue)
                        {
                            if (matchStack.size() == 1 && matchMap.count(itr.first + 4 * i)) {    
                                curmatchMap.insert({itr.first + 4 * i, itr.first});
                            }
                            else {
                                curmatchMap.insert({itr.first + 4 * i, itr.first});
                            }
                        }
                    }
                }
            }
        }
        else {
            for (auto &itr : matchMap)
            {
                neumrateNum++;
                vector<int> result(1);
                SIZE_T resultsize = 0;
                // LOGD("totalMem %d region num %d/%d", totalMem / 1024 / 1024, neumrateNum, processHeapMap.size());
                ReadProcessMemory(hProcess, (LPCVOID)(itr.first), &result[0],1, &resultsize);
                if (resultsize == 1) {
                    if (result[0] == ketvalue)
                    {
                        curmatchMap.insert({itr.first, itr.second});
                    }
                }
            }

        }
        LOGD("find ketvalue %d prekeyValue %d, curmatchMap %d matchMap %d", ketvalue, prekeyValue, curmatchMap.size(), matchMap.size());
        matchMap = curmatchMap;
        if (matchMap.size() == 1) {
            LOGD("find ketvalue %d addr %p, region addr %p: size %d", ketvalue, matchMap.begin()->first, matchMap.begin()->second, processHeapMap[matchMap.begin()->second]);
            break;
        }
    };

    int result[16];
    char* transPtr;
    SIZE_T resultsize = 0;
    {
        string input;
        LOGD("type in value to set :");
        getline(cin, input);
        int prekeyValue = matchStack.size() ? matchStack.top(): -1;
        ketvalue = atoi(input.c_str());
        WriteProcessMemory(hProcess, (LPVOID)matchMap.begin()->first, &ketvalue, sizeof(ketvalue), &resultsize);
    }
    LPCVOID ptr = (LPCVOID)0x23A67BEE978;
    ReadProcessMemory(hProcess, ptr, &transPtr, sizeof(transPtr), &resultsize);
    transPtr += 0x60;
    ReadProcessMemory(hProcess, transPtr, &result, 4, &resultsize);
    LOGD("got addr %p result %d", transPtr, result[0]);
    // 4.通过打开进程修改游戏内容
    // WriteProcessMemory(hProcess, (LPVOID)0x207FB5A0,
    //                    (LPVOID)&x, sizeof(x), &pid);
    // CloseHandle(snapShotHanlde);
    return 0;
}