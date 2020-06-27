#include "stdafx.h"
#include "main.h"
#include "Utils.h"

int main()
{
    //wchar_t kernel32[] = { 'k', 'e', 'r', 'n', 'e', 'l', '3', '2', '.', 'd', 'l', 'l', 0 };
    //volatile char EnterCriticalSection[] = { 'E', 'n', 't', 'e', 'r', 'C', 'r', 'i', 't', 'i', 'c', 'a', 'l', 'S', 'e', 'c', 't', 'i', 'o', 'n', 0 };
    //void* test = get_proc_address(get_module_handle(kernel32), (char*)EnterCriticalSection);

    // Path
    wchar_t kernel32[] = { 'k', 'e', 'r', 'n', 'e', 'l', '3', '2', '.', 'd', 'l', 'l', 0 };
    volatile char GetCurrentDirectoryW[] = { 'G', 'e', 't', 'C', 'u', 'r', 'r', 'e', 'n', 't', 'D', 'i', 'r', 'e', 'c', 't', 'o', 'r', 'y', 'W', 0 };
    GetCurrentDirectoryW_t GetCurrentDirectoryAddress = (GetCurrentDirectoryW_t)get_proc_address(get_module_handle(kernel32), (char*)GetCurrentDirectoryW);

    wchar_t path[MAX_PATH];
    GetCurrentDirectoryAddress(MAX_PATH, path);

    wchar_t Respondus[] = { '\\', 'R', 'e', 's', 'p', 'o', 'n', 'd', 'u', 's', '.', 'd', 'l', 'l', 0 };
    __wcscat(path, Respondus);

    // Load dll
    wchar_t ntdll[] = { 'n', 't', 'd', 'l', 'l', '.', 'd', 'l', 'l', 0 };
    char LdrLoadDll[] = { 'L', 'd', 'r', 'L', 'o', 'a', 'd', 'D', 'l', 'l', 0 };
    LdrLoadDll_t LdrLoadDllAddress = (LdrLoadDll_t)get_proc_address(get_module_handle(ntdll), LdrLoadDll);

    UNICODE_STRING fileName;
    __memset(&fileName, 0, sizeof(fileName));
    fileName.Length = static_cast<uint16_t>(__wcslen(path) * 2);
    fileName.MaximumLength = static_cast<uint16_t>(__wcslen(path) * 2) + 2;
    fileName.Buffer = path;
    HANDLE handle;
    LdrLoadDllAddress(nullptr, 0, &fileName, &handle);

	return 0;
}