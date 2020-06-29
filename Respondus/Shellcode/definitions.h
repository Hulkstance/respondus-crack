#pragma once

typedef LPVOID(WINAPI* VirtualAlloc_t)(
    _In_opt_ LPVOID lpAddress,
    _In_     SIZE_T dwSize,
    _In_     DWORD flAllocationType,
    _In_     DWORD flProtect);

typedef BOOL(WINAPI* VirtualFree_t)(
    _Pre_notnull_ _When_(dwFreeType == MEM_DECOMMIT, _Post_invalid_) _When_(dwFreeType == MEM_RELEASE, _Post_ptr_invalid_) LPVOID lpAddress,
    _In_ SIZE_T dwSize,
    _In_ DWORD dwFreeType);

typedef NTSTATUS(NTAPI* LdrLoadDll_t)(
    IN PWCHAR               PathToFile OPTIONAL,
    IN ULONG                Flags OPTIONAL,
    IN PUNICODE_STRING      ModuleFileName,
    OUT PHANDLE             ModuleHandle);

typedef HANDLE(WINAPI* GetProcessHeap_t)(
    VOID);

typedef DWORD(WINAPI* GetCurrentDirectoryW_t)(
    IN DWORD   nBufferLength,
    OUT LPWSTR lpBuffer);