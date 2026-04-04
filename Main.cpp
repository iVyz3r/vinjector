#include <windows.h>
#include <d3d9.h>
#include <string>
#include <fstream>
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
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"UWP Injector", WS_OVERLAPPEDWINDOW, 100, 100, 450, 350, NULL, NULL, wc.hInstance, NULL);

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

    // Variables de estado
    auto [saved_proc, saved_dll] = LoadConfig();
    static char proc_name[256] = {};
    static char dll_path[MAX_PATH] = {};
    std::string status_msg = "Listo";
    float status_color_r = 0.0f, status_color_g = 1.0f, status_color_b = 0.0f;

    strcpy_s(proc_name, sizeof(proc_name), saved_proc.c_str());
    strcpy_s(dll_path, sizeof(dll_path), saved_dll.c_str());

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

            ImGui::Text("Nombre del Proceso:");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##proc_name", proc_name, sizeof(proc_name), ImGuiInputTextFlags_CharsNoBlank);

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::Text("Archivo DLL:");
            ImGui::SetNextItemWidth(-120);
            ImGui::InputText("##dll_path", dll_path, sizeof(dll_path), ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();
            if (ImGui::Button("Seleccionar##dll", ImVec2(110, 0)))
            {
                OPENFILENAMEA ofn = {};
                char file_buffer[MAX_PATH] = {};
                strcpy_s(file_buffer, sizeof(file_buffer), dll_path);

                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = file_buffer;
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrFilter = "Archivos DLL\0*.dll\0Todos\0*.*\0";
                ofn.Flags = OFN_FILEMUSTEXIST;

                if (GetOpenFileNameA(&ofn))
                {
                    strcpy_s(dll_path, sizeof(dll_path), file_buffer);
                }
            }

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();

            static bool show_confirmation = false;
            if (ImGui::Button("Inyectar DLL", ImVec2(200, 40)))
            {
                show_confirmation = true;
            }

            ImGui::SameLine();
            ImGui::TextColored(ImVec4(status_color_r, status_color_g, status_color_b, 1.0f), status_msg.c_str());

            ImGui::Spacing();
            ImGui::Spacing();

            if (show_confirmation)
            {
                ImGui::OpenPopup("Confirmar Inyección");
            }

            if (ImGui::BeginPopupModal("Confirmar Inyección", &show_confirmation, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("¿Estás seguro de que quieres inyectar el DLL?");
                ImGui::Text("Proceso: %s", proc_name);
                ImGui::Text("DLL: %s", dll_path);

                if (ImGui::Button("Inyectar", ImVec2(120, 0)))
                {
                    SaveConfig(proc_name, dll_path);
                    
                    wchar_t wproc_buffer[256] = {};
                    MultiByteToWideChar(CP_ACP, 0, proc_name, -1, wproc_buffer, 256);
                    DWORD pid = ProcessUtils::GetProcessIdByName(wproc_buffer);

                    if (pid == 0)
                    {
                        status_msg = "❌ Proceso no encontrado";
                        status_color_r = 1.0f;
                        status_color_g = 0.0f;
                        status_color_b = 0.0f;
                    }
                    else if (strlen(dll_path) == 0)
                    {
                        status_msg = "❌ Selecciona un archivo DLL";
                        status_color_r = 1.0f;
                        status_color_g = 0.0f;
                        status_color_b = 0.0f;
                    }
                    else
                    {
                        try
                        {
                            wchar_t wdll_buffer[MAX_PATH] = {};
                            MultiByteToWideChar(CP_ACP, 0, dll_path, -1, wdll_buffer, MAX_PATH);
                            
                            if (Injector::ApplyUWPPermissions(wdll_buffer))
                            {
                                if (Injector::InjectDLL(pid, wdll_buffer))
                                {
                                    status_msg = "✓ Inyección exitosa - PID: " + std::to_string(pid);
                                    status_color_r = 0.0f;
                                    status_color_g = 1.0f;
                                    status_color_b = 0.0f;
                                }
                                else
                                {
                                    status_msg = "❌ Inyección fallida";
                                    status_color_r = 1.0f;
                                    status_color_g = 0.0f;
                                    status_color_b = 0.0f;
                                }
                            }
                            else
                            {
                                status_msg = "❌ No se pudieron aplicar permisos UWP";
                                status_color_r = 1.0f;
                                status_color_g = 0.0f;
                                status_color_b = 0.0f;
                            }
                        }
                        catch (const std::exception& e)
                        {
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
                if (ImGui::Button("Cancelar", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                    show_confirmation = false;
                }
                ImGui::EndPopup();
            }

            ImGui::End();
        }

        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx9 = D3DCOLOR_RGBA(45, 45, 48, 255);
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx9, 1.0f, 0);
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