#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <string>

namespace ProcessUtils {
    DWORD GetProcessIdByName(const std::wstring& processName);
}