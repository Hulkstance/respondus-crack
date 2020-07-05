#include "stdafx.h"
#include "main.h"
#include "Utils.h"

int main()
{
    // Path
    const wchar_t kernel32[] = { 'k', 'e', 'r', 'n', 'e', 'l', '3', '2', '.', 'd', 'l', 'l', 0 };
    volatile const char GetCurrentDirectoryW[] = { 'G', 'e', 't', 'C', 'u', 'r', 'r', 'e', 'n', 't', 'D', 'i', 'r', 'e', 'c', 't', 'o', 'r', 'y', 'W', 0 };
    GetCurrentDirectoryW_t GetCurrentDirectoryAddress = reinterpret_cast<GetCurrentDirectoryW_t>(get_proc_address(get_module_handle(kernel32), const_cast<char*>(GetCurrentDirectoryW)));
    if (!GetCurrentDirectoryAddress)
        return 0;

    wchar_t path[MAX_PATH];
    GetCurrentDirectoryAddress(MAX_PATH, path);

    const wchar_t Respondus[] = { '\\', 'R', 'e', 's', 'p', 'o', 'n', 'd', 'u', 's', '.', 'd', 'l', 'l', 0 };
    __wcscat(path, Respondus);

    // Load dll
    const wchar_t ntdll[] = { 'n', 't', 'd', 'l', 'l', '.', 'd', 'l', 'l', 0 };
    const char LdrLoadDll[] = { 'L', 'd', 'r', 'L', 'o', 'a', 'd', 'D', 'l', 'l', 0 };
    LdrLoadDll_t LdrLoadDllAddress = reinterpret_cast<LdrLoadDll_t>(get_proc_address(get_module_handle(ntdll), LdrLoadDll));
    if (!LdrLoadDllAddress)
        return 0;

    UNICODE_STRING fileName;
    __memset(&fileName, 0, sizeof(fileName));
    fileName.Length = static_cast<uint16_t>(__wcslen(path) * 2);
    fileName.MaximumLength = static_cast<uint16_t>(__wcslen(path) * 2) + 2;
    fileName.Buffer = path;
    HANDLE handle;
    LdrLoadDllAddress(nullptr, 0, &fileName, &handle);

	return 0;
}