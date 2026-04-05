#include "ProcessCheck.hpp"
#include <psapi.h>
#include <codecvt>
#include <locale>

#pragma comment(lib, "psapi.lib")

DWORD ProcessUtils::GetProcessIdByName(const std::wstring& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32W pe32 = {};
    pe32.dwSize = sizeof(pe32);

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (_wcsicmp(pe32.szExeFile, processName.c_str()) == 0) {
                CloseHandle(hSnapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return 0;
}

std::vector<ProcessInfo> ProcessUtils::GetAllProcesses() {
    std::vector<ProcessInfo> processes;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return processes;

    PROCESSENTRY32W pe32 = {};
    pe32.dwSize = sizeof(pe32);

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            ProcessInfo info;
            info.name = pe32.szExeFile;
            info.pid = pe32.th32ProcessID;
            
            // Intentar obtener la ruta del ejecutable
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe32.th32ProcessID);
            if (hProcess) {
                wchar_t path[MAX_PATH] = {};
                DWORD size = MAX_PATH;
                if (QueryFullProcessImageNameW(hProcess, 0, path, &size)) {
                    info.path = path;
                }
                CloseHandle(hProcess);
            }

            processes.push_back(info);
        } while (Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return processes;
}

std::vector<ModuleInfo> ProcessUtils::GetModulesByPID(DWORD pid) {
    std::vector<ModuleInfo> modules;

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess)
        return modules;

    HMODULE hMods[1024];
    DWORD cbNeeded;

    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        for (DWORD i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            wchar_t szModName[MAX_PATH] = {};
            if (GetModuleFileNameExW(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(wchar_t))) {
                ModuleInfo info;
                info.baseAddress = hMods[i];
                info.path = szModName;
                
                // Extraer solo el nombre del archivo
                size_t lastSlash = info.path.find_last_of(L"\\/");
                if (lastSlash != std::wstring::npos) {
                    info.name = info.path.substr(lastSlash + 1);
                } else {
                    info.name = info.path;
                }

                modules.push_back(info);
            }
        }
    }

    CloseHandle(hProcess);
    return modules;
}

bool ProcessUtils::UnloadDLLSafe(DWORD pid, const std::wstring& moduleName) {
    // Verificar que el proceso sigue vivo
    HANDLE hTestProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hTestProcess)
        return false;

    DWORD exitCode;
    if (!GetExitCodeProcess(hTestProcess, &exitCode) || exitCode != STILL_ACTIVE) {
        CloseHandle(hTestProcess);
        return false;
    }
    CloseHandle(hTestProcess);

    // Abrir proceso con permisos necesarios para CreateRemoteThread
    HANDLE hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | 
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, pid
    );
    if (!hProcess)
        return false;

    // Obtener la dirección de FreeLibrary en kernel32
    HMODULE hKernel32 = GetModuleHandleW(L"Kernel32.dll");
    if (!hKernel32) {
        CloseHandle(hProcess);
        return false;
    }

    FARPROC pFreeLibrary = GetProcAddress(hKernel32, "FreeLibrary");
    if (!pFreeLibrary) {
        CloseHandle(hProcess);
        return false;
    }

    // Encontrar el módulo a descargar
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        CloseHandle(hProcess);
        return false;
    }

    MODULEENTRY32W me32 = {};
    me32.dwSize = sizeof(me32);
    HMODULE hRemoteModule = NULL;
    bool found = false;

    if (Module32FirstW(hSnapshot, &me32)) {
        do {
            if (_wcsicmp(me32.szModule, moduleName.c_str()) == 0) {
                hRemoteModule = me32.hModule;
                found = true;
                break;
            }
        } while (Module32NextW(hSnapshot, &me32));
    }

    CloseHandle(hSnapshot);

    if (!found) {
        CloseHandle(hProcess);
        return false;
    }

    // Crear thread remoto para descargar la DLL
    HANDLE hRemoteThread = CreateRemoteThread(
        hProcess,
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)pFreeLibrary,
        hRemoteModule,
        0,
        NULL
    );

    if (!hRemoteThread) {
        CloseHandle(hProcess);
        return false;
    }

    // Esperar a que el thread termine (máximo 5 segundos)
    DWORD waitResult = WaitForSingleObject(hRemoteThread, 5000);
    CloseHandle(hRemoteThread);

    // Verificar resultado del thread
    DWORD threadExitCode = 0;
    if (waitResult == WAIT_OBJECT_0) {
        // Thread completado - exitoso
        CloseHandle(hProcess);
        return true;
    }

    // Timeout o error
    CloseHandle(hProcess);
    return false;
}

std::wstring ProcessUtils::GetModulePath(HMODULE hModule) {
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(hModule, path, MAX_PATH);
    return path;
}