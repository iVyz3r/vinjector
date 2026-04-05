#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>

struct ProcessInfo {
    std::wstring name;
    DWORD pid;
    std::wstring path;
};

struct ModuleInfo {
    std::wstring name;
    std::wstring path;
    HMODULE baseAddress;
};

// Utilidades de conversión
inline std::string WideToString(const std::wstring& wide) {
    if (wide.empty()) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), (int)wide.length(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), (int)wide.length(), &str[0], size_needed, NULL, NULL);
    return str;
}

namespace ProcessUtils {
    DWORD GetProcessIdByName(const std::wstring& processName);
    std::vector<ProcessInfo> GetAllProcesses();
    std::vector<ModuleInfo> GetModulesByPID(DWORD pid);
    bool UnloadDLLSafe(DWORD pid, const std::wstring& moduleName);
    std::wstring GetModulePath(HMODULE hModule);
}