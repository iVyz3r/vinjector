#include <windows.h>
#include <d3d9.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include "ProcessCheck/ProcessCheck.hpp"
#include "Inject/Inject.hpp"

#include "ImGui/imgui.h"
#include "ImGui/backend/imgui_impl_dx9.h"
#include "ImGui/backend/imgui_impl_win32.h"

#pragma comment(lib, "d3d9.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

std::string GetConfigPath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string path = buffer;
    return path.substr(0, path.find_last_of("\\/") + 1) + "inject.txt";
}

void SaveConfig(const std::string& proc, const std::string& dll) {
    std::ofstream file(GetConfigPath());
    if (file.is_open()) {
        file << proc << "\n" << dll << "\n";
        file.close();
    }
}

std::pair<std::string, std::string> LoadConfig() {
    std::ifstream file(GetConfigPath());
    std::string proc, dll;
    if (file.is_open()) {
        std::getline(file, proc);
        std::getline(file, dll);
        file.close();
    }
    return { proc, dll };
}

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

int main(int, char**)
{
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui UWP Injector", NULL };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"UWP Injector", WS_OVERLAPPEDWINDOW, 100, 100, 600, 620, NULL, NULL, wc.hInstance, NULL);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Cargar fuente Arial
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 16.0f);
    io.Fonts->Build();
    ImGui_ImplDX9_CreateDeviceObjects();

    // State variables
    auto [saved_proc, saved_dll] = LoadConfig();
    static char dll_path[MAX_PATH] = {};
    std::string status_msg = "Ready";
    float status_color_r = 0.0f, status_color_g = 1.0f, status_color_b = 0.0f;

    strcpy_s(dll_path, sizeof(dll_path), saved_dll.c_str());

    // New state for process and module selection
    static bool show_process_window = false;
    static DWORD selected_pid = 0;
    static std::wstring selected_proc_name = L"";
    static std::vector<ProcessInfo> process_list;
    static std::vector<ModuleInfo> module_list;
    static char process_search[256] = {};
    static bool show_modules = false;
    static int selected_module_index = -1;
    static auto last_process_check = std::chrono::steady_clock::now();
    static bool process_terminated = false;
    static auto last_process_list_update = std::chrono::steady_clock::now();

    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessageW(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
            ImGui::Begin("UWP DLL Injector", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

            ImGui::TextColored(ImVec4(0.1f, 0.7f, 1.0f, 1.0f), "UWP DLL Injector");
            ImGui::Separator();

            // Auto-detect if selected process terminated
            auto now = std::chrono::steady_clock::now();
            if (selected_pid > 0 && std::chrono::duration_cast<std::chrono::milliseconds>(now - last_process_check).count() > 1000) {
                last_process_check = now;
                HANDLE hTestProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, selected_pid);
                if (hTestProcess) {
                    DWORD exitCode;
                    if (!GetExitCodeProcess(hTestProcess, &exitCode) || exitCode != STILL_ACTIVE) {
                        process_terminated = true;
                    }
                    CloseHandle(hTestProcess);
                } else {
                    process_terminated = true;
                }

                if (process_terminated) {
                    status_msg = "⚠ Process terminated - Select another";
                    status_color_r = 1.0f;
                    status_color_g = 1.0f;
                    status_color_b = 0.0f;
                    selected_pid = 0;
                    selected_proc_name = L"";
                    show_modules = false;
                    module_list.clear();
                    selected_module_index = -1;
                    process_terminated = false;
                }
            }

            // Section: Select Process
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.0f, 1.0f), "1. Select Process");
            if (selected_pid > 0) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[PID: %lu] %S", selected_pid, selected_proc_name.c_str());
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "None selected");
            }
            ImGui::SameLine();
            if (ImGui::Button("Select Process##btn", ImVec2(130, 0))) {
                show_process_window = true;
                process_search[0] = '\0';
                process_list = ProcessUtils::GetAllProcesses();
            }

            ImGui::Separator();

            // Section: Modules
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.0f, 1.0f), "2. Manage Modules");
            if (selected_pid > 0) {
                if (ImGui::Button("Show Modules", ImVec2(120, 32))) {
                    show_modules = !show_modules;
                    if (show_modules) {
                        selected_module_index = -1;
                        module_list = ProcessUtils::GetModulesByPID(selected_pid);
                    }
                }
            } else {
                ImGui::BeginDisabled();
                ImGui::Button("Show Modules", ImVec2(120, 32));
                ImGui::EndDisabled();
            }

            // Loaded modules
            if (show_modules && selected_pid > 0) {
                ImGui::TextColored(ImVec4(0.1f, 0.7f, 1.0f, 1.0f), "Modules (%d):", (int)module_list.size());
                ImGui::SetNextItemWidth(-1);
                ImGui::BeginChild("ModulesList", ImVec2(0, 100), true, ImGuiWindowFlags_None);

                for (int i = 0; i < (int)module_list.size(); i++) {
                    bool is_selected = (selected_module_index == i);
                    if (ImGui::Selectable(WideToString(module_list[i].name).c_str(), is_selected)) {
                        selected_module_index = i;
                    }

                    if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                        selected_module_index = i;
                    }
                }

                ImGui::EndChild();

                // Context menu for modules
                if (selected_module_index >= 0 && selected_module_index < (int)module_list.size()) {
                    if (ImGui::BeginPopupContextItem("ModuleContextMenu")) {
                        if (ImGui::MenuItem("Unload DLL")) {
                            if (ProcessUtils::UnloadDLLSafe(selected_pid, module_list[selected_module_index].name)) {
                                status_msg = "[-] DLL unloaded successfully";
                                status_color_r = 0.0f;
                                status_color_g = 1.0f;
                                status_color_b = 0.0f;
                                module_list = ProcessUtils::GetModulesByPID(selected_pid);
                                selected_module_index = -1;
                            } else {
                                status_msg = "[X] Error unloading DLL";
                                status_color_r = 1.0f;
                                status_color_g = 0.0f;
                                status_color_b = 0.0f;
                            }
                        }

                        if (ImGui::MenuItem("Open Location")) {
                            // Get module path and open in Explorer
                            std::wstring modulePath = module_list[selected_module_index].path;
                            ShellExecuteW(NULL, L"open", L"explorer.exe", (L"/select,\"" + modulePath + L"\"").c_str(), NULL, SW_SHOW);
                        }

                        ImGui::EndPopup();
                    }
                }

                ImGui::Spacing();
                ImGui::Separator();
            }

            // Section: Inject DLL
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.0f, 1.0f), "3. Inject DLL");
            ImGui::Text("DLL File:");
            ImGui::SetNextItemWidth(-110);
            ImGui::InputText("##dll_path", dll_path, sizeof(dll_path), ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();
            if (ImGui::Button("Browse##dll", ImVec2(100, 0)))
            {
                OPENFILENAMEA ofn = {};
                char file_buffer[MAX_PATH] = {};
                strcpy_s(file_buffer, sizeof(file_buffer), dll_path);

                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = file_buffer;
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrFilter = "DLL Files\0*.dll\0All\0*.*\0";
                ofn.Flags = OFN_FILEMUSTEXIST;

                if (GetOpenFileNameA(&ofn))
                {
                    strcpy_s(dll_path, sizeof(dll_path), file_buffer);
                }
            }

            static bool show_confirmation = false;
            if (selected_pid > 0) {
                if (ImGui::Button("Inject DLL", ImVec2(120, 32))) {
                    show_confirmation = true;
                }
            } else {
                ImGui::BeginDisabled();
                ImGui::Button("Inject DLL", ImVec2(120, 32));
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            ImGui::TextColored(ImVec4(status_color_r, status_color_g, status_color_b, 1.0f), status_msg.c_str());

            // Confirm dialog
            if (show_confirmation) {
                ImGui::OpenPopup("Confirm Injection");
            }

            if (ImGui::BeginPopupModal("Confirm Injection", &show_confirmation, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Are you sure you want to inject the DLL?");
                ImGui::Text("Process: %S (PID: %lu)", selected_proc_name.c_str(), selected_pid);
                ImGui::Text("DLL: %s", dll_path);

                if (ImGui::Button("Inject", ImVec2(120, 0))) {
                    char proc_name_buffer[256] = {};
                    sprintf_s(proc_name_buffer, sizeof(proc_name_buffer), "%S", selected_proc_name.c_str());
                    SaveConfig(proc_name_buffer, dll_path);

                    if (strlen(dll_path) == 0) {
                        status_msg = "❌ Select a DLL file";
                        status_color_r = 1.0f;
                        status_color_g = 0.0f;
                        status_color_b = 0.0f;
                    } else {
                        try {
                            wchar_t wdll_buffer[MAX_PATH] = {};
                            MultiByteToWideChar(CP_ACP, 0, dll_path, -1, wdll_buffer, MAX_PATH);
                            
                            if (Injector::ApplyUWPPermissions(wdll_buffer)) {
                                if (Injector::InjectDLL(selected_pid, wdll_buffer)) {
                                    status_msg = "✓ Injection successful - PID: " + std::to_string(selected_pid);
                                    status_color_r = 0.0f;
                                    status_color_g = 1.0f;
                                    status_color_b = 0.0f;
                                } else {
                                    status_msg = "❌ Injection failed";
                                    status_color_r = 1.0f;
                                    status_color_g = 0.0f;
                                    status_color_b = 0.0f;
                                }
                            } else {
                                status_msg = "❌ Could not apply UWP permissions";
                                status_color_r = 1.0f;
                                status_color_g = 0.0f;
                                status_color_b = 0.0f;
                            }
                        } catch (const std::exception& e) {
                            status_msg = std::string("❌ Error: ") + e.what();
                            status_color_r = 1.0f;
                            status_color_g = 0.0f;
                            status_color_b = 0.0f;
                        }
                    }

                    ImGui::CloseCurrentPopup();
                    show_confirmation = false;
                }

                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                    show_confirmation = false;
                }

                ImGui::EndPopup();
            }

            ImGui::End();
        }

        // Process selection window
        if (show_process_window) {
            // Auto-update process list every 2 seconds
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - last_process_list_update).count() >= 2) {
                last_process_list_update = now;
                process_list = ProcessUtils::GetAllProcesses();
            }

            ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Select Process", &show_process_window, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Search process:");
                ImGui::SetNextItemWidth(-1);
                ImGui::InputText("##process_search", process_search, sizeof(process_search));

                ImGui::Spacing();
                ImGui::Text("Active processes:");
                ImGui::SetNextItemWidth(-1);
                ImGui::BeginChild("ProcessList", ImVec2(480, 250), true, ImGuiWindowFlags_None);

                for (size_t i = 0; i < process_list.size(); i++) {
                    std::string proc_display = WideToString(process_list[i].name) + " (PID: " + std::to_string(process_list[i].pid) + ")";
                    
                    // Apply search filter
                    if (strlen(process_search) > 0) {
                        std::string search_lower = process_search;
                        std::transform(search_lower.begin(), search_lower.end(), search_lower.begin(), ::tolower);
                        std::string proc_lower = proc_display;
                        std::transform(proc_lower.begin(), proc_lower.end(), proc_lower.begin(), ::tolower);
                        
                        if (proc_lower.find(search_lower) == std::string::npos) {
                            continue;
                        }
                    }

                    if (ImGui::Selectable(proc_display.c_str(), selected_pid == process_list[i].pid)) {
                        selected_pid = process_list[i].pid;
                        selected_proc_name = process_list[i].name;
                        show_process_window = false;
                        show_modules = false;
                        selected_module_index = -1;
                        module_list.clear();
                    }
                }

                ImGui::EndChild();

                if (ImGui::Button("Cancel", ImVec2(100, 0))) {
                    show_process_window = false;
                }

                ImGui::End();
            }
        }

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}