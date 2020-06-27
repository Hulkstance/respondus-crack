#include "Misc.h"

typedef HHOOK(WINAPI* SetWindowsHookExA_t)(int, HOOKPROC, HINSTANCE, DWORD);
typedef LSTATUS(WINAPI* RegQueryValueExA_t)(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
typedef LSTATUS(WINAPI* RegOpenKeyExA_t)(HKEY, LPCSTR, DWORD, REGSAM, PHKEY);
typedef HDEVINFO(WINAPI* SetupDiGetClassDevsW_t)(const GUID*, PCWSTR, HWND, DWORD);
typedef DWORD(WINAPI* GetEnvironmentVariableA_t)(LPCSTR, LPSTR, DWORD);
typedef HWND(WINAPI* CreateWindowExW_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
typedef BOOL(WINAPI* ShowWindow_t)(HWND, int);
typedef BOOL(WINAPI* SetWindowPos_t)(HWND, HWND, int, int, int, int, UINT);

SetWindowsHookExA_t OriginalSetWindowsHookExA = nullptr;
RegQueryValueExA_t OriginalRegQueryValueExA = nullptr;
RegOpenKeyExA_t OriginalRegOpenKeyExA = nullptr;
SetupDiGetClassDevsW_t OriginalSetupDiGetClassDevsW = nullptr;
GetEnvironmentVariableA_t OriginalGetEnvironmentVariableA = nullptr;
CreateWindowExW_t OriginalCreateWindowExW = nullptr;
ShowWindow_t OriginalShowWindow = nullptr;
SetWindowPos_t OriginalSetWindowPos = nullptr;

/*
Enables ALT + TAB.
*/
HHOOK WINAPI DetourSetWindowsHookExA(
    int       idHook,
    HOOKPROC  lpfn,
    HINSTANCE hmod,
    DWORD     dwThreadId
)
{
    return OriginalSetWindowsHookExA(0, nullptr, nullptr, 0);
}

/*
Anti VM.

RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\DESCRIPTION\System", 0, KEY_READ, 0x002eeeb4)
RegQueryValueExA(0x00000640, "SystemBiosVersion", NULL, 0x002eee9c, 0x002ef2d4, 0x002eeeac)
RegQueryValueExA(0x00000640, "VideoBiosVersion", NULL, 0x002eee9c, 0x002ef2d4, 0x002eeeac)

RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\DESCRIPTION\System\BIOS", 0, KEY_READ, 0x002eeeb4)
RegQueryValueExA(0x00000640, "SystemManufacturer", NULL, 0x002eee9c, 0x002ef2d4, 0x002eeeac)

RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\DESCRIPTION\System\CentralProcessor\0", 0, KEY_READ, 0x002eeeb4)
RegQueryValueExA(0x00000640, "ProcessorNameString", NULL, 0x002eee9c, 0x002ef2d4, 0x002eeeac)

*/
LSTATUS WINAPI DetourRegQueryValueExA(
    HKEY    hKey,
    LPCSTR  lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE  lpData,
    LPDWORD lpcbData
)
{
    const char* values[] = {
        "SystemBiosVersion",
        "VideoBiosVersion",
        "SystemManufacturer",
        "ProcessorNameString"
    };

    for (int i = 0; i < ARRAYSIZE(values); i++)
    {
        if (StrStrIA(lpValueName, values[i]))
        {
            printf("RegQueryValueExA: %s\n", lpValueName);
            return 0x2;
        }
    }

    return OriginalRegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

/*
Anti VM.

RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\DEVICEMAP\Scsi\Scsi Port 1", 0, KEY_READ, 0x002eeeb4)

*/
LSTATUS WINAPI DetourRegOpenKeyExA(
    HKEY   hKey,
    LPCSTR lpSubKey,
    DWORD  ulOptions,
    REGSAM samDesired,
    PHKEY  phkResult
)
{
    if (StrStrIA(lpSubKey, "Scsi\\Scsi Port 1"))
    {
        printf("RegOpenKeyExA: %s\n", lpSubKey);
        return 0x2;
    }

    return OriginalRegOpenKeyExA(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

/*
Anti VM.
*/
HDEVINFO WINAPI DetourSetupDiGetClassDevsW(
    const GUID* ClassGuid,
    PCWSTR     Enumerator,
    HWND       hwndParent,
    DWORD      Flags
)
{
    return INVALID_HANDLE_VALUE;
}

/*
Anti VM.

GetEnvironmentVariableA("CAMEYO_VIRTUALAPP", 0x002ced08, 1020)
GetEnvironmentVariableA("CAMEYO_RO_VIRTUALAPP", 0x002ced08, 1020)
GetEnvironmentVariableA("CAMEYO_RO_PROPERTY_VIRTUALAPP", 0x002ced08, 1020)

*/
DWORD WINAPI DetourGetEnvironmentVariableA(
    LPCSTR lpName,
    LPSTR  lpBuffer,
    DWORD  nSize
)
{
    if (StrStrIA(lpName, "CAMEYO"))
    {
        return 0;
    }

    return OriginalGetEnvironmentVariableA(lpName, lpBuffer, nSize);
}

/*
A minimize button, because they forgot to add one :)
*/
void KeybindThread(HWND hWnd)
{
    while (1)
    {
        if (GetAsyncKeyState(VK_NUMPAD0) & 1)
        {
            printf("Num0 was pressed.\n");

            ShowWindow(hWnd, SW_MINIMIZE);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

HWND WINAPI DetourCreateWindowExW(
    DWORD     dwExStyle,
    LPCWSTR   lpClassName,
    LPCWSTR   lpWindowName,
    DWORD     dwStyle,
    int       X,
    int       Y,
    int       nWidth,
    int       nHeight,
    HWND      hWndParent,
    HMENU     hMenu,
    HINSTANCE hInstance,
    LPVOID    lpParam
)
{
    // TOPMOST gayshit
    dwExStyle &= ~WS_EX_TOPMOST;

    HWND hWnd = OriginalCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    // Main browser window
    // I could use FindWindowW(L"LOCKDOWNCHROME", L"Respondus LockDown Browser") instead, but hooking is faster.
    if (dwExStyle == WS_EX_APPWINDOW)
    {
        printf("CreateWindowExW: %ws (Class name) | %ws (Window name)\n", lpClassName, lpWindowName);
    
        std::thread tHotKey(KeybindThread, hWnd);
        tHotKey.detach();
    }

    return hWnd;
}

/*
Taskbar/Start button hiding.

FindWindowW("Shell_TrayWnd", NULL)
ShowWindow(0x0001005e, SW_HIDE)
FindWindowExW(NULL, NULL, "Button", "Start")
ShowWindow(0x00010062, SW_HIDE)

*/
BOOL WINAPI DetourShowWindow(
    HWND hWnd,
    int  nCmdShow
)
{
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    HWND hStartButton = FindWindowExW(nullptr, nullptr, L"Button", L"Start");

    if (hWnd == hTaskbar || hWnd == hStartButton)
    {
        return FALSE;
    }

    return OriginalShowWindow(hWnd, nCmdShow);
}

/*
Second TOPMOST gayshit.

SetWindowPos(0x00140428, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW)

*/
BOOL WINAPI DetourSetWindowPos(
    HWND hWnd,
    HWND hWndInsertAfter,
    int  X,
    int  Y,
    int  cx,
    int  cy,
    UINT uFlags
)
{
    if (hWndInsertAfter == HWND_TOPMOST)
    {
        hWndInsertAfter = HWND_BOTTOM;
    }

    return OriginalSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

void Initialize()
{
    AllocConsole();

    FILE* fDummy = nullptr;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);

    SetConsoleTitleW(L"Respondus");
}

bool Hook()
{
    if (MH_Initialize() != MH_OK)
        return false;

    // SetWindowsHookExA hook
    LPVOID lpSetWindowsHookExA = GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowsHookExA");
    if (!lpSetWindowsHookExA)
        return false;

    if (MH_CreateHook(lpSetWindowsHookExA, &DetourSetWindowsHookExA, reinterpret_cast<LPVOID*>(&OriginalSetWindowsHookExA)) != MH_OK)
        return false;

    if (MH_EnableHook(lpSetWindowsHookExA) != MH_OK)
        return false;

    printf("SetWindowsHookExA successfully hooked.\n");
    
    // RegQueryValueExA hook
    LPVOID lpRegQueryValueExA = GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "RegQueryValueExA");
    if (!lpRegQueryValueExA)
        return false;

    if (MH_CreateHook(lpRegQueryValueExA, &DetourRegQueryValueExA, reinterpret_cast<LPVOID*>(&OriginalRegQueryValueExA)) != MH_OK)
        return false;

    if (MH_EnableHook(lpRegQueryValueExA) != MH_OK)
        return false;

    printf("RegQueryValueExA successfully hooked.\n");

    // RegOpenKeyExA hook
    LPVOID lpRegOpenKeyExA = GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "RegOpenKeyExA");
    if (!lpRegOpenKeyExA)
        return false;

    if (MH_CreateHook(lpRegOpenKeyExA, &DetourRegOpenKeyExA, reinterpret_cast<LPVOID*>(&OriginalRegOpenKeyExA)) != MH_OK)
        return false;

    if (MH_EnableHook(lpRegOpenKeyExA) != MH_OK)
        return false;

    printf("RegOpenKeyExA successfully hooked.\n");

    // SetupDiGetClassDevsW hook
    LPVOID lpSetupDiGetClassDevsW = GetProcAddress(LoadLibraryW(L"SetupAPI.dll"), "SetupDiGetClassDevsW");
    if (!lpSetupDiGetClassDevsW)
        return false;

    if (MH_CreateHook(lpSetupDiGetClassDevsW, &DetourSetupDiGetClassDevsW, reinterpret_cast<LPVOID*>(&OriginalSetupDiGetClassDevsW)) != MH_OK)
        return false;

    if (MH_EnableHook(lpSetupDiGetClassDevsW) != MH_OK)
        return false;

    printf("SetupDiGetClassDevsW successfully hooked.\n");

    // GetEnvironmentVariableA hook
    LPVOID lpGetEnvironmentVariableA = GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetEnvironmentVariableA");
    if (!lpGetEnvironmentVariableA)
        return false;

    if (MH_CreateHook(lpGetEnvironmentVariableA, &DetourGetEnvironmentVariableA, reinterpret_cast<LPVOID*>(&OriginalGetEnvironmentVariableA)) != MH_OK)
        return false;

    if (MH_EnableHook(lpGetEnvironmentVariableA) != MH_OK)
        return false;

    printf("GetEnvironmentVariableA successfully hooked.\n");

    // CreateWindowExW hook
    LPVOID lpCreateWindowExW = GetProcAddress(GetModuleHandleW(L"user32.dll"), "CreateWindowExW");
    if (!lpCreateWindowExW)
        return false;

    if (MH_CreateHook(lpCreateWindowExW, &DetourCreateWindowExW, reinterpret_cast<LPVOID*>(&OriginalCreateWindowExW)) != MH_OK)
        return false;

    if (MH_EnableHook(lpCreateWindowExW) != MH_OK)
        return false;

    printf("CreateWindowExW successfully hooked.\n");

    // ShowWindow hook
    LPVOID lpShowWindow = GetProcAddress(GetModuleHandleW(L"user32.dll"), "ShowWindow");
    if (!lpShowWindow)
        return false;

    if (MH_CreateHook(lpShowWindow, &DetourShowWindow, reinterpret_cast<LPVOID*>(&OriginalShowWindow)) != MH_OK)
        return false;

    if (MH_EnableHook(lpShowWindow) != MH_OK)
        return false;

    printf("ShowWindow successfully hooked.\n");

    // SetWindowPos hook
    LPVOID lpSetWindowPos = GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowPos");
    if (!lpSetWindowPos)
        return false;

    if (MH_CreateHook(lpSetWindowPos, &DetourSetWindowPos, reinterpret_cast<LPVOID*>(&OriginalSetWindowPos)) != MH_OK)
        return false;

    if (MH_EnableHook(lpSetWindowPos) != MH_OK)
        return false;

    printf("SetWindowPos successfully hooked.\n");

    printf("====================\n");

    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Initialize();
        Hook();
        break;
    }
    return TRUE;
}