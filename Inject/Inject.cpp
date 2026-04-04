#include "Inject.hpp"
#include <sddl.h>
#include <aclapi.h>

bool Injector::ApplyUWPPermissions(const std::wstring& dllPath) {
    PACL pOldDACL = NULL, pNewDACL = NULL;
    PSECURITY_DESCRIPTOR pSD = NULL;
    EXPLICIT_ACCESSW ea = { 0 };

    if (GetNamedSecurityInfoW(dllPath.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDACL, NULL, &pSD) != ERROR_SUCCESS)
        return false;

    BuildExplicitAccessWithNameW(&ea, (LPWSTR)L"ALL APPLICATION PACKAGES", GENERIC_READ | GENERIC_EXECUTE, SET_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);
    SetEntriesInAclW(1, &ea, pOldDACL, &pNewDACL);
    DWORD result = SetNamedSecurityInfoW((LPWSTR)dllPath.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDACL, NULL);
    
    if (pSD) LocalFree(pSD);
    if (pNewDACL) LocalFree(pNewDACL);
    return result == ERROR_SUCCESS;
}

bool Injector::InjectDLL(DWORD pid, const std::wstring& dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) return false;

    size_t size = (dllPath.length() + 1) * sizeof(wchar_t);
    LPVOID pBuf = VirtualAllocEx(hProcess, NULL, size, MEM_COMMIT, PAGE_READWRITE);
    if (!pBuf) { CloseHandle(hProcess); return false; }

    WriteProcessMemory(hProcess, pBuf, dllPath.c_str(), size, NULL);
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW"), pBuf, 0, NULL);

    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }
    VirtualFreeEx(hProcess, pBuf, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return hThread != NULL;
}