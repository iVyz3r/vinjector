#pragma once
#include <windows.h>
#include <string>

namespace Injector {
    bool ApplyUWPPermissions(const std::wstring& dllPath);
    bool InjectDLL(DWORD pid, const std::wstring& dllPath);
}