#include <iostream>
#include <windows.h>
#include <thread>
#include <d3d9.h>
#pragma comment(lib,"d3d9.lib")

#include "Menu.h"
#include "../Globals.h"
#include "../Server/Server.hpp"
#include "../../Security/xorstr.hpp"
#include "../../Security/base64.h"

void CMenu::DrawMainForm()
{
   /* static int OldWindowSizeX, OldWindowSizeY = 0;
    if (g_Globals->m_iGlobalWindowSizeX < 700 || g_Globals->m_iGlobalWindowSizeY < 500)
        SetWindowPos(g_Globals->main_hwnd, 0, g_Globals->m_iGlobalWindowPosX, g_Globals->m_iGlobalWindowPosY, OldWindowSizeX, OldWindowSizeY, 0);

    OldWindowSizeX = g_Globals->m_iGlobalWindowSizeX;
    OldWindowSizeY = g_Globals->m_iGlobalWindowSizeY;*/

    static bool WindowActive = true;
    ImGui::SetNextWindowPos(ImVec2(g_Globals->m_iGlobalWindowPosX, g_Globals->m_iGlobalWindowPosY));
    ImGui::SetNextWindowSize(ImVec2(g_Globals->m_iGlobalWindowSizeX, g_Globals->m_iGlobalWindowSizeY));
    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::Begin(Xorstr("Anime helper"), &WindowActive, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    g_Menu->wp = ImGui::GetWindowPos();
    g_Menu->ws = ImGui::GetWindowSize();

    g_Menu->Tabs();
    g_Menu->SubTabs();

    if (g_Menu->CurrentTab == 0)
    {
        if (g_User->UserName.length() < 2)
        {
            g_Menu->FormSize = ImVec2(300, 250);

            if (g_Menu->CurrentSubTab == 0)
            {
                g_Menu->SubTabLogin();
            }
            else if (g_Menu->CurrentSubTab == 1)
            {
                g_Menu->SubTabRegister();
            }
        }
    }
    else if (g_Menu->CurrentTab == 1)
    {
        g_Menu->FormSize = ImVec2(1410, 1015);

        if (g_Menu->CurrentSubTab == 0)
        {
            g_Menu->SubTabAll();
        }
        else if (g_Menu->CurrentSubTab == 1)
        {
            g_Menu->SubTabMy();
        }
    }

    ImGui::End();
}


void CMenu::Tabs()
{
    ImGui::TabButton(Xorstr("Home"), &g_Menu->CurrentTab, 0, 2);

    if (g_User->UserName.length() > 2)
        ImGui::TabButton(Xorstr("Account"), &g_Menu->CurrentTab, 1, 2);
}

void CMenu::SubTabs()
{
    switch (g_Menu->CurrentTab)
    {
    case 0:
        if (g_User->UserName.length() < 2)
        {
            ImGui::SubTabButton(Xorstr("Login"), &g_Menu->CurrentSubTab, 0, 2);
            ImGui::SubTabButton(Xorstr("Register"), &g_Menu->CurrentSubTab, 1, 2);
        }
        break;
    case 1:
        ImGui::SubTabButton(Xorstr("All"), &g_Menu->CurrentSubTab, 0, 2);
        ImGui::SubTabButton(Xorstr("My"), &g_Menu->CurrentSubTab, 1, 2);
        break;
    }
}

void CMenu::SubTabLogin()
{
    ImGui::SetNextWindowPos(ImVec2(wp.x + 160, wp.y + 40));
    ImGui::BeginChild(Xorstr("LoginForm"), ImVec2(ws.x - 160, ws.y - 40));

    static char aUserName[128];
    static char aPassword[128];

    ImGui::InputText(Xorstr("Username"), aUserName, 128);
    ImGui::InputText(Xorstr("Password"), aPassword, 128, ImGuiInputTextFlags_Password);

    if (ImGui::Button(Xorstr("Login"), ImVec2(0, 30)))
    {
        g_Server->Login(aUserName, aPassword);
    }

    ImGui::EndChild();
}

void CMenu::SubTabRegister()
{
    ImGui::SetNextWindowPos(ImVec2(wp.x + 160, wp.y + 40));
    ImGui::BeginChild(Xorstr("RegisterForm"), ImVec2(ws.x - 160, ws.y - 40));

    static char aUserName[128];
    static char aPassword[128];

    ImGui::InputText(Xorstr("Username"), aUserName, 128);
    ImGui::InputText(Xorstr("Password"), aPassword, 128, ImGuiInputTextFlags_Password);

    if (ImGui::Button(Xorstr("Register"), ImVec2(0, 30)))
    {
        g_Server->Register(aUserName, aPassword);
    }

    ImGui::EndChild();
}
std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); } // correct
    );
    return s;
}
bool FilterString(std::string ourstring, std::string filter)
{
    std::string lower_our = str_tolower(ourstring);
    std::string lower_filter = str_tolower(filter);

    for (int i = 0; i < lower_our.size(); i++)
    {
        int n = lower_our.find(lower_filter, i);
        if (n != std::string::npos)
            return true;
        else
            break;
    }
    return false;
}

void CMenu::SubTabAll()
{
    ImGui::SetNextWindowPos(ImVec2(wp.x + 10, wp.y + 150));
    ImGui::BeginChild(Xorstr("filtertab"), ImVec2(150, 500));
    static char aNameFilter[128];
    ImGui::InputText(Xorstr("Name Filter"), aNameFilter, 128);
    std::string strNameFilter = aNameFilter;
    ImGui::EndChild();

    ImGui::SetNextWindowPos(ImVec2(wp.x + 160, wp.y + 10));
    ImGui::BeginChild(Xorstr("all anime"), ImVec2(ws.x - 160, ws.y - 10));

    int CurrentItemForSameLine = 1;
   
    g_Menu->ResizeImagesByWindowSize();

    for (int i = 0; i < g_Globals->AllAnimeList.size(); i++)
    {
        if (!FilterString(g_Globals->AllAnimeList.at(i).name, strNameFilter) && strNameFilter.size() > 0)
            continue;

        if (ImGui::CustomButtonImage(g_Globals->AllAnimeList.at(i).name.c_str(), g_Globals->AllAnimeList.at(i).desc.c_str(), m_vecImageSize, !g_Globals->AllAnimeList.at(i).texture, g_Globals->AllAnimeList.at(i).texture, i))
        {
            g_Server->SaveAnimeList();
        }

        if (ImGui::IsItemVisible() && !g_Globals->AllAnimeList.at(i).texture)
        {
            g_Globals->GetTextureAllAnimeList(i);
        }

        if (!ImGui::IsItemVisible() && g_Globals->AllAnimeList.at(i).texture)
        {
            g_Globals->AllAnimeList.at(i).texture->Release();
            g_Globals->AllAnimeList.at(i).texture = nullptr;
        }

        if (CurrentItemForSameLine >= g_Menu->SameLineMax) {
            CurrentItemForSameLine = 1;
        }
        else {
            CurrentItemForSameLine++;
            ImGui::SameLine();
        }
    }

    for (int i = 0; i < g_Globals->AnimeFavorites.size(); i++)
    {
        if (g_Globals->AnimeFavorites.at(i).texture)
        {
            g_Globals->AnimeFavorites.at(i).texture->Release();
            g_Globals->AnimeFavorites.at(i).texture = nullptr;
        }
    }

    g_Globals->m_flScrollAmount = ImGui::GetScrollY();
    ImGui::EndChild();
}

std::string CMenu::ClampName(const char* label, int max_charpters) 
{
    std::string ret = std::string(label);

    if (ret.size() >= max_charpters) {
        while (ret.size() >= max_charpters)
            ret.pop_back();
        ret += "...";
    }

    return ret;
};

void CMenu::SubTabMy()
{
    ImGui::SetNextWindowPos(ImVec2(wp.x + 160, wp.y + 10));
    ImGui::BeginChild(Xorstr("Favorites"), ImVec2(ws.x - 160, ws.y - 10));

    int CurrentItemForSameLine = 1;

    g_Menu->ResizeImagesByWindowSize();

    if (ProgressTab == 0)
    {
        for (int i = 0; i < g_Globals->AnimeFavorites.size(); i++)
        {
            if (ImGui::CustomButtonImage(g_Globals->AnimeFavorites.at(i).name.c_str(), g_Globals->AnimeFavorites.at(i).desc.c_str(), m_vecImageSize, !g_Globals->AnimeFavorites.at(i).texture, g_Globals->AnimeFavorites.at(i).texture, i, 1))
            {
                CurrentSelectedName = g_Globals->AnimeFavorites.at(i).name;
                CurrentSelectedIndex = i;
                ShellExecuteA(NULL, "open", g_Globals->GetAnimeUrl(&g_Globals->AnimeFavorites, i).c_str(), NULL, NULL, SW_SHOWNORMAL);
                g_Server->SaveAnimeList();
            }

            if (ImGui::IsItemVisible() && !g_Globals->AnimeFavorites.at(i).texture)
            {
                g_Globals->GetTextureFavotites(i);
            }

            if (!ImGui::IsItemVisible() && g_Globals->AnimeFavorites.at(i).texture)
            {
                g_Globals->AnimeFavorites.at(i).texture->Release();
                g_Globals->AnimeFavorites.at(i).texture = nullptr;
            }

            if (CurrentItemForSameLine >= g_Menu->SameLineMax) {
                CurrentItemForSameLine = 1;
            }
            else {
                CurrentItemForSameLine++;
                ImGui::SameLine();
            }
        }
    }
    if (ProgressTab == 1)
    {

    }

    for (int i = 0; i < g_Globals->AllAnimeList.size(); i++)
    {
        if (g_Globals->AllAnimeList.at(i).texture)
        {
            g_Globals->AllAnimeList.at(i).texture->Release();
            g_Globals->AllAnimeList.at(i).texture = nullptr;
        }
    }

    ImGui::EndChild();
}

void CMenu::ResizeImagesByWindowSize()
{
    ImVec2 m_vecImageMinSize = ImVec2(200, 320);//1,6
    ImVec2 m_vecImageMaxSize = ImVec2(250, 400);
    ImVec2 m_vecChildSize = ImGui::GetWindowSize();
    ImVec2 m_vecComfortImageSize = ImVec2((ImGui::GetWindowSize().x - 7 * (g_Menu->SameLineMax + 1)) / g_Menu->SameLineMax, (ImGui::GetWindowSize().x - 7 * (g_Menu->SameLineMax + 1)) / g_Menu->SameLineMax * 1.6f);

    if (m_vecComfortImageSize.x > m_vecImageMaxSize.x)
    {
        g_Menu->m_vecImageSize = m_vecImageMaxSize;
        g_Menu->SameLineMax += 1;
    }
    else
    {
        if (m_vecComfortImageSize.x > m_vecImageMinSize.x)
        {
            g_Menu->m_vecImageSize = m_vecComfortImageSize;
        }
        else
        {
            g_Menu->SameLineMax -= 1;
            g_Menu->m_vecImageSize = m_vecImageMinSize;
        }
    }
}