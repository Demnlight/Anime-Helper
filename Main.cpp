#include "Main.h"
#include "Data/json.hpp"
#include "Data/fontbytearray.h"

#include "Backend/Globals.h"

#include "Security/Security.h"
#include "Security/xorstr.hpp"
#include "Security/lazy_importer.hpp"

#pragma warning(disable : 4996)

void Security()
{
#if _DEBUG
    g_Logger->StartUp();
#endif // _DEBUG

    g_Security->Init();
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
    main_hwnd = CreateWindow(wc.lpszClassName, Xorstr("Anime helper"), WS_POPUP, 0, 0, 720, 360, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(main_hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return;
    }

    // Show the window
    ShowWindow(main_hwnd, SW_HIDE);
    UpdateWindow(main_hwnd);

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

        style.ScrollbarSize = 3.f;
        style.ScrollbarRounding = 5.f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(main_hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    //Load Fonts
    {
        io.Fonts->AddFontFromMemoryTTF((void*)segoeui, sizeof(segoeui), 15.0f, NULL, io.Fonts->GetGlyphRangesDefault());
        io.Fonts->AddFontFromMemoryTTF((void*)segoeui, sizeof(segoeui), 25.0f, NULL, io.Fonts->GetGlyphRangesDefault());
        io.Fonts->AddFontFromMemoryTTF((void*)segoeui, sizeof(segoeui), 20.0f, NULL, io.Fonts->GetGlyphRangesDefault());
    }

    DWORD window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    RECT screen_rect;
    GetWindowRect(GetDesktopWindow(), &screen_rect);
    int x = 10/*float(screen_rect.right - WINDOW_WIDTH) / 2.f*/;
    int y = 10/*float(screen_rect.bottom - WINDOW_HEIGHT) / 2.f*/;

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

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {

            ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Once);
            ImGui::SetNextWindowSizeConstraints(ImVec2(1410, 1015), ImVec2(1410, 2560));
            ImGui::SetNextWindowBgAlpha(1.0f);
            ImGui::Begin(Xorstr("Anime helper"), &loader_active, window_flags);
            auto wp = ImGui::GetWindowPos();
            auto ws = ImGui::GetWindowSize();

            static int current_tab = 0;
            //Tabs
            {
                ImGui::TabButton(Xorstr("Home"), &current_tab, 0, 2);
                ImGui::TabButton(Xorstr("Account"), &current_tab, 1, 2);
            }
            static int current_sub_tab = 0;
            //SubTabs
            {
                switch (current_tab)
                {
                case 0:
                    //ImGui::SubTabButton(Xorstr("Search"), &current_sub_tab, 0, 2);
                    //ImGui::SubTabButton(Xorstr("Account"), &current_sub_tab, 1, 2);
                    break;
                case 1:
                    ImGui::SubTabButton(Xorstr("All"), &current_sub_tab, 0, 2);
                    ImGui::SubTabButton(Xorstr("My"), &current_sub_tab, 1, 2);
                    break;
                }
            }

            //Func
            {
                if (current_tab == 0)
                {

                }
                if (current_tab == 1)
                {
                    if (current_sub_tab == 0)
                    {
                        static int ProgressTab = 0;
                        const int every_inventory_item = 6;
                        int current_inventory_item = 1;

                        ImGui::SetNextWindowPos(ImVec2(wp.x + 160, wp.y + 40));
                        ImGui::BeginChild(Xorstr("all anime"), ImVec2(ws.x - 160, ws.y - 40));
                        static std::string CurrentSelectedName = Xorstr("CurrentSelectedIndex");

                        if (ProgressTab == 0)
                        {
                            current_inventory_item = 2;

                            if (ImGui::CustomButton(Xorstr("+"), ImVec2(200, 320)))
                            {
                                ProgressTab = 1;
                            }

                            ImGui::SameLine();

                            for (int i = 0; i < g_Globals->AllAnimeList.size(); i++)
                            {
                                auto texture = g_Globals->GetTexture(&g_Globals->AllAnimeList, i);
                                if (ImGui::CustomButtonImage(g_Globals->AllAnimeList.at(i).name.c_str(), g_Globals->AllAnimeList.at(i).desc.c_str(), ImVec2(200, 320), !texture, texture, i))
                                {
                                    g_AnimeList->SaveAnimeList();
                                }

                                if (current_inventory_item >= every_inventory_item) {
                                    current_inventory_item = 1;
                                }
                                else {
                                    current_inventory_item++;
                                    ImGui::SameLine();
                                }
                            }
                        }
                        if (ProgressTab == 1)
                        {
                            static char aAnimeName[128];
                            static char aAnimeDesc[2048];
                            static char aAnimeImageUrl[256];
                            static char aAnimeRating[128];
                            static char type[128];
                            static char status[128];
                            static char genre[128];
                            static char original_source[128];
                            static char last_season_date_release[128];
                            static char start_and_end_date[128];
                            static char studio[128];
                            static char MPAA_rating[128];
                            static char age_restrictions[128];
                            static char average_part_duration[128];
                            static char ranobe_manga[128];

                            ImGui::InputText(Xorstr("Anime name"), aAnimeName, 128);
                            ImGui::InputTextMultiline(Xorstr("Anime Desc"), aAnimeDesc, 2048, ImVec2(0, 200));
                            ImGui::InputText(Xorstr("Anime Image Url"), aAnimeImageUrl, 128);
                            ImGui::InputText(Xorstr("Anime Rating"), aAnimeRating, 3, ImGuiInputTextFlags_CharsDecimal);
                            ImGui::InputText(Xorstr("Anime Type"), type, 128);
                            ImGui::InputText(Xorstr("Anime Status"), status, 128);
                            ImGui::InputText(Xorstr("Anime Genre"), genre, 128);
                            ImGui::InputText(Xorstr("Anime Original source"), original_source, 128);
                            ImGui::InputText(Xorstr("Anime Last season date release"), last_season_date_release, 128);
                            ImGui::InputText(Xorstr("Anime Release"), start_and_end_date, 128);
                            ImGui::InputText(Xorstr("Anime Studio"), studio, 128);
                            ImGui::InputText(Xorstr("Anime MPAA rating"), MPAA_rating, 128);
                            ImGui::InputText(Xorstr("Anime Age restrictions"), age_restrictions, 128);
                            ImGui::InputText(Xorstr("Anime Average part duration"), average_part_duration, 128);
                            ImGui::InputText(Xorstr("Anime Ranobe/Manga"), ranobe_manga, 128);

                            if (ImGui::Button(Xorstr("Add to list")))
                            {
                                AnimeList anime;
                                anime.name = aAnimeName;
                                anime.desc = aAnimeDesc;
                                anime.image_url = aAnimeImageUrl;
                                anime.rating = std::stoi(aAnimeRating);
                                anime.status = status;
                                anime.genre = genre;
                                anime.original_source = original_source;
                                anime.last_season_date_release = last_season_date_release;
                                anime.start_and_end_date = start_and_end_date;
                                anime.studio = studio;
                                anime.MPAA_rating = MPAA_rating;
                                anime.age_restrictions = age_restrictions;
                                anime.average_part_duration = average_part_duration;
                                anime.ranobe_manga = ranobe_manga;

                                g_Globals->AllAnimeList.push_back(anime);

                                g_Internet->DownloadImage(g_Globals->AllAnimeList.size() - 1);
                                g_AnimeList->SaveAnimeList();
                                memset(aAnimeName, 0, sizeof aAnimeName);
                                memset(aAnimeDesc, 0, sizeof aAnimeDesc);
                                memset(aAnimeImageUrl, 0, sizeof aAnimeImageUrl);
                                memset(aAnimeRating, 0, sizeof aAnimeRating);
                                memset(status, 0, sizeof status);
                                memset(genre, 0, sizeof genre);
                                memset(original_source, 0, sizeof original_source);
                                memset(last_season_date_release, 0, sizeof last_season_date_release);
                                memset(start_and_end_date, 0, sizeof start_and_end_date);
                                memset(studio, 0, sizeof studio);
                                memset(MPAA_rating, 0, sizeof MPAA_rating);
                                memset(age_restrictions, 0, sizeof age_restrictions);
                                memset(average_part_duration, 0, sizeof average_part_duration);
                                memset(ranobe_manga, 0, sizeof ranobe_manga);
                                ProgressTab = 0;
                            }
                            if (ImGui::IsKeyPressedMap(ImGuiKey_Escape))
                                ProgressTab = 0;
                        }

                        ImGui::EndChild();
                    }
                    if (current_sub_tab == 1)
                    {
                        const int every_inventory_item = 4;
                        int current_inventory_item = 1;
                        current_inventory_item = 1;

                        static int ProgressTab = 0;
                        static std::string CurrentSelectedName = Xorstr("CurrentSelectedIndex");
                        static int CurrentSelectedIndex = 0;
                        if (ProgressTab == 0)
                        {
                            ImGui::SetNextWindowPos(ImVec2(wp.x + 160, wp.y + 40));
                            ImGui::BeginChild(Xorstr("Favorites"), ImVec2(ws.x - 160, ws.y - 40));

                            for (int i = 0; i < g_Globals->AnimeFavorites.size(); i++)
                            {
                                auto texture = g_Globals->GetTexture(&g_Globals->AnimeFavorites, i);
                                if (ImGui::CustomButtonImage(g_Globals->AnimeFavorites.at(i).name.c_str(), g_Globals->AnimeFavorites.at(i).desc.c_str(), ImVec2(200, 320), !texture, texture, i, 1))
                                {
                                    ProgressTab = 1;
                                    CurrentSelectedIndex = i;
                                    g_AnimeList->SaveAnimeList();
                                }

                                if (current_inventory_item >= every_inventory_item) {
                                    current_inventory_item = 1;
                                }
                                else {
                                    current_inventory_item++;
                                    ImGui::SameLine();
                                }
                            }
                            ImGui::EndChild();
                        }
                        if (ProgressTab == 1)
                        {
                            ImGui::SetNextWindowPos(ImVec2(wp.x + 160, wp.y + 40));
                            ImGui::BeginChild(Xorstr("Image"), ImVec2(200, ws.y - 40));

                            auto texture = g_Globals->GetTexture(&g_Globals->AnimeFavorites, CurrentSelectedIndex);

                            ImGui::Image(texture, ImVec2(200, 320));
                            ImGui::EndChild();

                            ImGui::SetNextWindowPos(ImVec2(wp.x + 160 + 200 + 10, wp.y + 40));
                            ImGui::BeginChild(Xorstr("Text Func"), ImVec2(ws.x - 160, ws.y - 40));

                            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
                            ImGui::TextColored(ImColor(255, 255, 255, 255), g_Globals->AnimeFavorites.at(g_Globals->FindInMainArrayIndexByName(CurrentSelectedName, g_Globals->AnimeFavorites)).name.c_str());
                            ImGui::PopFont();

                            std::string output_description = g_Globals->AutomaticNewStroke(g_Globals->AnimeFavorites.at(g_Globals->FindInMainArrayIndexByName(CurrentSelectedName, g_Globals->AnimeFavorites)).desc, ws.x - 390);
                            ImGui::TextColored(ImColor(200, 200, 200, 255), ClampName(output_description.c_str(), 150).c_str());

                            ImGui::InputInt(Xorstr("Anime Rating"), &g_Globals->AnimeFavorites.at(g_Globals->FindInMainArrayIndexByName(CurrentSelectedName, g_Globals->AnimeFavorites)).rating, 128);
                            ImGui::InputText(Xorstr("Anime last_season_saw"), g_Globals->AnimeFavorites.at(g_Globals->FindInMainArrayIndexByName(CurrentSelectedName, g_Globals->AnimeFavorites)).last_season_saw.data(), 128);
                            ImGui::InputText(Xorstr("Anime last_series_saw"), g_Globals->AnimeFavorites.at(g_Globals->FindInMainArrayIndexByName(CurrentSelectedName, g_Globals->AnimeFavorites)).last_series_saw.data(), 128);
                            ImGui::InputText(Xorstr("Anime last_time_saw"), g_Globals->AnimeFavorites.at(g_Globals->FindInMainArrayIndexByName(CurrentSelectedName, g_Globals->AnimeFavorites)).last_time_saw.data(), 128);
                            ImGui::InputText(Xorstr("Anime link_for_watching"), g_Globals->AnimeFavorites.at(g_Globals->FindInMainArrayIndexByName(CurrentSelectedName, g_Globals->AnimeFavorites)).link_for_watching.data(), 128);

                            if (ImGui::Button(Xorstr("Watch"), ImVec2(200, 30)))
                            {
                                ShellExecute(NULL, Xorstr("open"), g_Globals->AnimeFavorites.at(g_Globals->FindInMainArrayIndexByName(CurrentSelectedName, g_Globals->AnimeFavorites)).link_for_watching.c_str(), NULL, NULL, SW_SHOWNORMAL);
                            }
                            ImGui::EndChild();

                            if (ImGui::IsKeyPressedMap(ImGuiKey_Escape))
                            {
                                g_AnimeList->SaveAnimeList();
                                CurrentSelectedIndex = 0;

                                ProgressTab = 0;
                            }
                        }
                    }
                }
            }

            ImGui::End();
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
        if (!loader_active) {
            msg.message = WM_QUIT;
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(main_hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

#if _DEBUG
    g_Logger->Detach();
#endif // _DEBUG
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    std::thread drawing(Drawing);
    std::thread globals(Globals);
    std::thread security(Security);

    security.join();
    globals.join();
    drawing.join();

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}