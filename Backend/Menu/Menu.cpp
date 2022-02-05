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
    DWORD WindowFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiCond_Once);
    ImGui::SetNextWindowSizeConstraints(g_Menu->FormSize, g_Menu->FormSize);
    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::Begin(Xorstr("Anime helper"), &WindowActive, WindowFlags);
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

void CMenu::SubTabAll()
{
    ImGui::SetNextWindowPos(ImVec2(wp.x + 160, wp.y + 40));
    ImGui::BeginChild(Xorstr("all anime"), ImVec2(ws.x - 160, ws.y - 40));

    if (ProgressTab == 0)
    {
        int CurrentItemForSameLine = 2;

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
            g_Server->SaveAnimeList();
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
    if (ProgressTab == 0)
    {
        int CurrentItemForSameLine = 1;
        ImGui::SetNextWindowPos(ImVec2(wp.x + 160, wp.y + 40));
        ImGui::BeginChild(Xorstr("Favorites"), ImVec2(ws.x - 160, ws.y - 40));

        for (int i = 0; i < g_Globals->AnimeFavorites.size(); i++)
        {
            auto texture = g_Globals->GetTexture(&g_Globals->AnimeFavorites, i);
            if (ImGui::CustomButtonImage(g_Globals->AnimeFavorites.at(i).name.c_str(), g_Globals->AnimeFavorites.at(i).desc.c_str(), ImVec2(200, 320), !texture, texture, i, 1))
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
    if (ProgressTab == 1)
    {
        ImGui::SetNextWindowPos(ImVec2(wp.x + 160, wp.y + 40));
        ImGui::BeginChild(Xorstr("Image"), ImVec2(200, ws.y - 40));

        auto texture = g_Globals->GetTexture(&g_Globals->AnimeFavorites, g_Globals->FindInMainArrayIndexByName(CurrentSelectedName, g_Globals->AnimeFavorites));

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
            g_Server->SaveAnimeList();
            CurrentSelectedName = "";

            ProgressTab = 0;
        }
    }
}