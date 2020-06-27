#pragma once

#include <Windows.h>
#include <winternl.h>
#include <stdio.h>
#include <stdint.h>
#include <thread>

// MinHook
#include "MinHook/include/MinHook.h"

#if defined _M_IX86
#pragma comment(lib, "MinHook\\lib\\libMinHook.x86.lib")
#elif defined _M_X64
#pragma comment(lib, "MinHook\\lib\\libMinHook.x64.lib")
#endif

// Setup APIs
#include <SetupAPI.h>

// StrStrI
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")
