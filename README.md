# Respondus Bypass

This project is made only for educational purposes. It bypasses LockDown Browser, made by Respondus.

# Features
- Removes hooks from key combinations like `ALT + TAB`
- Bypasses anti VM checks (Tested on VMware)
- Removes TOPMOST flag from the splash screen
- Adds a minimize button keybind (`NumPad0`)
- Prevents start button and task bar hiding

# Projects description
- Loader - starts LockDown Browser in a suspended state, so it can hook their APIs before LockDown Browser gets annoying with its TOPMOST flags, key hooks, etc.
- Respondus - a DLL responsible for all features described above. It uses MinHook.
- Shellcode - loads `Respondus.dll` using `LdrLoadDll`. The reason behind LdrLoadDll is because we have to start the process in a suspended state, which means only `ntdll.dll` is loaded in memory at that time and we can't use for example kernel32 Win APIs. The DLL name is hard-coded, but you can easily change that.
- MinHook - use it to compile MinHook for your own Platform Toolset and then replace `C:\Path\Respondus\Respondus\MinHook\lib\libMinHook.x86.lib` and `C:\Path\Respondus\Respondus\MinHook\lib\libMinHook.x64.lib` with the new files

# Credits
- TsudaKageyu - for MinHook
- Ch40zz
