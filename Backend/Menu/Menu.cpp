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
    static bool WindowActive = true;
    ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiCond_Once);
    ImGui::SetNextWindowSize(g_Menu->FormSize);
    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::Begin(Xorstr("Anime helper"), &WindowActive, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
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
        {
            return true;
        }
        else
        {
            break;
        }
    }
    return false;
}

void CMenu::SubTabAll()
{
    ImGui::SetNextWindowPos(ImVec2(wp.x + 10, wp.y + 180));
    ImGui::BeginChild(Xorstr("filtertab"), ImVec2(150, 500));
    static char aNameFilter[128];
    ImGui::InputText(Xorstr("Name Filter"), aNameFilter, 128);
    std::string strNameFilter = aNameFilter;
    ImGui::EndChild();

    ImGui::SetNextWindowPos(ImVec2(wp.x + 160, wp.y + 40));
    ImGui::BeginChild(Xorstr("all anime"), ImVec2(ws.x - 160, ws.y - 40));

    int CurrentItemForSameLine = 1;
    int ItemLine = -3;

    for (int i = 0; i < g_Globals->AllAnimeList.size(); i++)
    {
        if (!FilterString(g_Globals->AllAnimeList.at(i).name, strNameFilter) && strNameFilter.size() > 0)
            continue;

        if (ItemLine * 320 < g_Globals->m_flScrollAmount)
            g_Globals->GetTextureAllAnimeList(i);

        if (ImGui::CustomButtonImage(g_Globals->AllAnimeList.at(i).name.c_str(), g_Globals->AllAnimeList.at(i).desc.c_str(), ImVec2(200, 320), !g_Globals->AllAnimeList.at(i).texture, g_Globals->AllAnimeList.at(i).texture, i))
        {
            g_Server->SaveAnimeList();
        }

        if (CurrentItemForSameLine >= SameLineMax) {
            CurrentItemForSameLine = 1;
            ItemLine++;
        }
        else {
            CurrentItemForSameLine++;
            ImGui::SameLine();
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
    ImGui::SetNextWindowPos(ImVec2(wp.x + 160, wp.y + 40));
    ImGui::BeginChild(Xorstr("Favorites"), ImVec2(ws.x - 160, ws.y - 40));

    int CurrentItemForSameLine = 1;
    int ItemLine = -3;

    for (int i = 0; i < g_Globals->AnimeFavorites.size(); i++)
    {
        if (ItemLine * 320 < g_Globals->m_flScrollAmount)
            g_Globals->GetTextureFavotites(i);

        if (ImGui::CustomButtonImage(g_Globals->AnimeFavorites.at(i).name.c_str(), g_Globals->AnimeFavorites.at(i).desc.c_str(), ImVec2(200, 320), !g_Globals->AnimeFavorites.at(i).texture, g_Globals->AnimeFavorites.at(i).texture, i, 1))
        {
            //ProgressTab = 1;
            CurrentSelectedName = g_Globals->AnimeFavorites.at(i).name;
            g_Server->SaveAnimeList();
        }

        if (CurrentItemForSameLine >= SameLineMax) {
            CurrentItemForSameLine = 1;
        }
        else {
            CurrentItemForSameLine++;
            ImGui::SameLine();
        }
    }
    ImGui::EndChild();
}