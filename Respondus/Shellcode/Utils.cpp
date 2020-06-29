#include "stdafx.h"
#include "Utils.h"

void* __malloc(size_t size)
{
    wchar_t kernel32[] = { 'k', 'e', 'r', 'n', 'e', 'l', '3', '2', '.', 'd', 'l', 'l', 0 };
    char _VirtualAlloc[] = { 'V', 'i', 'r', 't', 'u', 'a', 'l', 'A', 'l', 'l', 'o', 'c', 0 };
    VirtualAlloc_t VirtualAlloc = reinterpret_cast<VirtualAlloc_t>(get_proc_address(get_module_handle(kernel32), _VirtualAlloc));

    if (!VirtualAlloc)
        return nullptr;

    return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

BOOL __free(void* block)
{
    wchar_t kernel32[] = { 'k', 'e', 'r', 'n', 'e', 'l', '3', '2', '.', 'd', 'l', 'l', 0 };
    char _VirtualFree[] = { 'V', 'i', 'r', 't', 'u', 'a', 'l', 'F', 'r', 'e', 'e', 0 };
    VirtualFree_t VirtualFree = reinterpret_cast<VirtualFree_t>(get_proc_address(get_module_handle(kernel32), _VirtualFree));

    if (!VirtualFree)
        return FALSE;

    return VirtualFree(block, 0, MEM_RELEASE);
}

void* __memcpy(void* dst, void* src, size_t size)
{
    char* source = reinterpret_cast<char*>(src);
    char* destination = reinterpret_cast<char*>(dst);

    size_t i = size;

    while (i-- > 0)
        *destination++ = *source++;

    return dst;
}

void* __memset(void* src, int val, size_t size)
{
    char* source = reinterpret_cast<char*>(src);

    while (size > 0)
    {
        *source = val;
        source++;
        size--;
    }

    return src;
}

size_t __strlen(const char* str)
{
    const char* s;

    for (s = str; *s; ++s)
        ;

    return s - str;
}

size_t __wcslen(const wchar_t* s)
{
    const wchar_t* p = s;

    while (*p)
        p++;

    return p - s;
}

char* __strdup(const char* s)
{
    size_t len = __strlen(s) + 1;
    char* copy = reinterpret_cast<char*>(__malloc(len * sizeof(char)));

    if (!copy)
        return nullptr;

    __memcpy(copy, const_cast<char*>(s), len);

    return copy;
}

char* __strchr(const char* p, int32_t ch)
{
    char c = ch;

    for (;; ++p)
    {
        if (*p == c)
            return const_cast<char*>(p);
        if (!*p)
            return nullptr;
    }
}

wint_t __towlower(wint_t c)
{
    if (c >= 0x41 && c <= 0x5a)
        return c | 0x60;

    return c;
}

int32_t __wcsicmp(const wchar_t* s1, const wchar_t* s2)
{
    wchar_t c1, c2;

    for (; *s1; s1++, s2++)
    {
        c1 = __towlower(*s1);
        c2 = __towlower(*s2);
        if (c1 != c2)
            return static_cast<int32_t>(c1 - c2);
    }
    return -*s2;
}

wchar_t* __wcscat(wchar_t* s1, const wchar_t* s2)
{
    wchar_t* cp = s1;

    while (*cp)
        cp++;

    while (*cp++ = *s2++)
        ;

    return s1;
}

wchar_t* ascii_to_unicode(char* ascii, wchar_t* memory, size_t memory_size)
{
    wchar_t* unicode = memory;

    while (*memory++ = *ascii++)
        memory_size--;

    return unicode;
}

void* get_module_handle(const wchar_t* moduleName)
{
#if defined _M_IX86
    PPEB pPEB = reinterpret_cast<PPEB>(__readfsdword(0x30));
#elif defined _M_X64
    PPEB pPEB = reinterpret_cast<PPEB>(__readgsqword(0x60));
#endif

    for (PLIST_ENTRY pListEntry = pPEB->Ldr->InMemoryOrderModuleList.Flink; pListEntry && pListEntry != &pPEB->Ldr->InMemoryOrderModuleList; pListEntry = pListEntry->Flink)
    {
        PLDR_DATA_TABLE_ENTRY pLdrDataTableEntry = CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        if (!__wcsicmp(pLdrDataTableEntry->BaseDllName.Buffer, moduleName))
            return pLdrDataTableEntry->DllBase;
    }

    return nullptr;
}

void* get_proc_address(void* module, const char* functionName)
{
    if (!module)
        return nullptr;

    PIMAGE_DOS_HEADER pDOSHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
    
#if defined _M_IX86
    PIMAGE_NT_HEADERS32 pNTHeader = reinterpret_cast<PIMAGE_NT_HEADERS32>(reinterpret_cast<uint8_t*>(module) + pDOSHeader->e_lfanew);
#elif defined _M_X64
    PIMAGE_NT_HEADERS64 pNTHeader = reinterpret_cast<PIMAGE_NT_HEADERS64>(reinterpret_cast<uint8_t*>(module) + pDOSHeader->e_lfanew);
#endif
    
    if (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE || pNTHeader->Signature != IMAGE_NT_SIGNATURE)
        return nullptr;

    PIMAGE_EXPORT_DIRECTORY pExport = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(reinterpret_cast<uint8_t*>(module) + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    uint16_t* AddressOfNameOrdinals = reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(module) + pExport->AddressOfNameOrdinals);
    uint32_t* AddressOfNames = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(module) + pExport->AddressOfNames);
    uint32_t* AddressOfFunctions = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(module) + pExport->AddressOfFunctions);

    for (uint32_t i = 0; i < pExport->NumberOfNames; i++)
    {
        char* exportName = reinterpret_cast<char*>(reinterpret_cast<uint8_t*>(module) + AddressOfNames[i]);

        if (!strcmp(exportName, functionName))
        {
            char* exportAddress = reinterpret_cast<char*>(reinterpret_cast<uint8_t*>(module) + AddressOfFunctions[AddressOfNameOrdinals[i]]);

            // Forwarders
            if (AddressOfFunctions[AddressOfNameOrdinals[i]] >= pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress &&
                AddressOfFunctions[AddressOfNameOrdinals[i]] < pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
            {
                char* forwardLib = __strdup(exportAddress);
                char* forwardName = __strchr(forwardLib, '.');
                *forwardName++ = 0;

                size_t size = __strlen(forwardLib) * sizeof(wchar_t);
                wchar_t* allocatedMemory = reinterpret_cast<wchar_t*>(__malloc(size));
                wchar_t* unicode = ascii_to_unicode(forwardLib, allocatedMemory, size);
                wchar_t dll[] = { '.', 'd', 'l', 'l', 0 };
                __wcscat(unicode, dll);

                void* pModule = get_module_handle(unicode);

                if (!pModule)
                    return nullptr;

                __free(allocatedMemory);

                return get_proc_address(pModule, forwardName);
            }

            return exportAddress;
        }
    }

    return nullptr;
}