#pragma once

size_t __wcslen(const wchar_t* s);
void* get_module_handle(const wchar_t* moduleName);
void* get_proc_address(void* module, const char* functionName);
void* __memset(void* src, int val, size_t count);
wchar_t* __wcscat(wchar_t* s1, const wchar_t* s2);