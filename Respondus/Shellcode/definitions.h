#pragma once

typedef PVOID(NTAPI* RtlAllocateHeap_t)(
    IN PVOID  HeapHandle,
    IN ULONG  Flags,
    IN SIZE_T Size);

typedef BOOLEAN(NTAPI* RtlFreeHeap_t)(
    IN PVOID HeapHandle,
    IN ULONG Flags OPTIONAL,
    IN PVOID MemoryPointer);

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