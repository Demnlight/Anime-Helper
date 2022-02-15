#include "Main.h"
#include "Data/json.hpp"
#include "Data/fontbytearray.h"

#include "Backend/Globals.h"
#include "Backend/Menu/Menu.h"
#include "Backend/Server/Server.hpp"

#include "Security/Security.h"
#include "Security/xorstr.hpp"
#include "Security/lazy_importer.hpp"
#include "Security/base64.h"
#include <tchar.h>

#pragma warning(disable : 4996)

/*
TODO:
1) Fix Favorite settings save

*/

void Security()
{
    _TCHAR szPath[] = _T("Software\\AnimeHelper\\");
    std::string token = g_Crypt->base64_encode(g_User->GetHWID());
    std::string hwid = g_User->GetHWID();

    HKEY hKey;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, szPath, 0, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
        return;
    }
    if (RegSetValueEx(hKey, _T("Token"), NULL, REG_SZ, (LPBYTE)token.c_str(), strlen(token.c_str()) + 1) != ERROR_SUCCESS) {
        return;
    }
    if (RegSetValueEx(hKey, _T("Hwid"), NULL, REG_SZ, (LPBYTE)hwid.c_str(), strlen(hwid.c_str()) + 1) != ERROR_SUCCESS) {
        return;
    }
    if (RegCloseKey(hKey) != ERROR_SUCCESS) {
        return;
    }

    //g_Security->Init();
}
void Globals()
{
    g_Globals->Init();
}

void Drawing()
{
    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, Xorstr("Anime helper"), NULL };
    RegisterClassEx(&wc);
    //main_hwnd = CreateWindow(wc.lpszClassName, Xorstr("Anime helper"), WS_POPUP, 0, 0, 5, 5, NULL, NULL, wc.hInstance, NULL);
    g_Globals->main_hwnd = CreateWindowEx(0, wc.lpszClassName, Xorstr("Anime helper"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(g_Globals->main_hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return;
    }

    // Show the window
    ShowWindow(g_Globals->main_hwnd, SW_SHOWDEFAULT /*SW_HIDE*/);
    UpdateWindow(g_Globals->main_hwnd);

    // Setup Dear ImGui context
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; //crutial for not leaving the imgui.ini file
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;

        style.ScrollbarSize = 5.f;
        style.ScrollbarRounding = 5.f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_Globals->main_hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    //Load Fonts
    {
        io.Fonts->AddFontFromMemoryTTF((void*)segoeui, sizeof(segoeui), 15.0f, NULL, io.Fonts->GetGlyphRangesDefault());
        io.Fonts->AddFontFromMemoryTTF((void*)segoeui, sizeof(segoeui), 25.0f, NULL, io.Fonts->GetGlyphRangesDefault());
        io.Fonts->AddFontFromMemoryTTF((void*)segoeui, sizeof(segoeui), 20.0f, NULL, io.Fonts->GetGlyphRangesDefault());
    }

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        g_Globals->ProceedAnimeListAll();
        g_Globals->ProceedFavoriteAnime();

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            g_Menu->DrawMainForm();
        }
        ImGui::EndFrame();

        g_Globals->g_pd3dDevice = g_pd3dDevice;
        g_Globals->g_d3dpp = g_d3dpp;
        g_Globals->g_pD3D = g_pD3D;

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ResetDevice();
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(g_Globals->main_hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

#if _DEBUG
    g_Logger->Detach();
#endif // _DEBUG
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#if _DEBUG
    g_Logger->StartUp();
#endif // _DEBUG


    g_Server->Instance();

    std::thread SecurityThread(Security);
    std::thread DrawingThread(Drawing);
    std::thread GlobalsThread(Globals);
    
    SecurityThread.join();
    DrawingThread.join();
    GlobalsThread.join();

    return 0;
}

BOOL WindowResizing(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    RECT rect = { NULL };
    GetClientRect(hWnd, &rect);

    // Offered width and height
    g_Globals->m_iGlobalWindowSizeX = rect.right - rect.left;
    g_Globals->m_iGlobalWindowSizeY = rect.bottom - rect.top;
    return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_MOVE:
    {
        g_Globals->m_iGlobalWindowPosX = (int)(short)LOWORD(lParam);
        g_Globals->m_iGlobalWindowPosY = (int)(short)HIWORD(lParam);
        break;
    }
    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = 1460;
        lpMMI->ptMinTrackSize.y = 720;
        break;
    }
    //case WM_SIZING:
    //{
    //    WindowResizing(hWnd, wParam, lParam);
    //    break;
    //}
    case WM_SIZE:
    {
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        g_Globals->m_iGlobalWindowSizeX = (int)(short)LOWORD(lParam);
        g_Globals->m_iGlobalWindowSizeY = (int)(short)HIWORD(lParam);
    }
    break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}