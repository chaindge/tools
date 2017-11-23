#pragma once
#include <windows.h>
#include <vector>

DWORD RunCmd(const char* path, bool show_window);

DWORD GetParantPid(DWORD dwId);

void GetChildPids(DWORD pid, std::vector<DWORD>& vec_child);

void init_ntdll();

void ListModule(void);

void list_process();