#include "Globals.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx9.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_internal.h"
#include "Server/Server.hpp"

#include <fstream>
#include <algorithm>
#include <D3dx9tex.h>
#include <Shlobj.h>
#include <thread>
#pragma comment(lib, "D3dx9")

#define ARRAY_SEARCH(array, elem) (std::find(array.begin(), array.end(), elem) != array.end())

void C_Globals::Init()
{    
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, g_Globals->AppDataPath)))
    {
        std::string FinalPath           = g_Globals->AppDataPath;         FinalPath += "\\AnimeHelper\\";
        std::string FinalPathImages     = FinalPath;                      FinalPathImages += "\\Images\\";
        CreateDirectory(FinalPath.c_str(), NULL);
        CreateDirectory(FinalPathImages.c_str(), NULL);
    }

    g_Server->Login(g_User->GetHWID());

    g_Globals->CheckVersion();

    g_Server->PushAnimeList(0);
    g_Server->PushAnimeList(1);

    g_Globals->FormatAnimeName(&g_Globals->AllAnimeList);
    g_Globals->FormatAnimeName(&g_Globals->AnimeFavorites);

    for (int i = 0; i < g_Globals->AllAnimeList.size(); i++)
        g_Internet->DownloadImage(i);

  /*  for (int i = 0; i < g_Globals->AllAnimeList.size(); i++)
    {
        g_Globals->GetTexture(&g_Globals->AllAnimeList, i);
        Sleep(200);
    }*/
}
void C_Globals::FormatAnimeName(std::vector<AnimeList>* arr)
{
    for (int index = 0; index < arr->size(); index++)
    {
        for (int i = 0; i < arr->at(index).name.length(); i++)
            if (arr->at(index).name.at(i) == '?' ||
                arr->at(index).name.at(i) == ':' ||
                arr->at(index).name.at(i) == '`' ||
                arr->at(index).name.at(i) == '"' ||
                arr->at(index).name.at(i) == '/')
                arr->at(index).name.at(i) = ' ';
    }
}
void C_Globals::CheckVersion()
{
    auto loader_version = g_Internet->GetUrlData(Xorstr("animehelper.000webhostapp.com"), Xorstr("version.php"));
    std::string version = Xorstr("1.003");

    g_Logger->SetColor(COLOR_DEBUG);
    printf("[ Check Version ]");
    g_Logger->ResetColor();

    printf(" Current Version: %s | Server Version: %s \n", version.c_str(), loader_version.c_str());

    if (loader_version != version)
    {
        g_Logger->SetColor(COLOR_DEBUG);
        printf("[ Check Version ]");
        g_Logger->ResetColor();
        printf(" Update \n");

        if (MessageBox(nullptr, Xorstr("Version Oudated, Please wait"), Xorstr("Version outdated"), 0))
        {
            std::remove(Xorstr("delete.exe"));
            rename(Xorstr("AnimeHelper.exe"), Xorstr("delete.exe"));
            URLDownloadToFile(NULL, Xorstr("https://animehelper.000webhostapp.com/AnimeHelper.exe"), Xorstr("AnimeHelper.exe"), 0, NULL);
            ShellExecute(NULL, Xorstr("open"), Xorstr("AnimeHelper.exe"), NULL, NULL, SW_SHOWNORMAL);
            std::remove(Xorstr("delete.exe"));
            exit(0);
        }
    }
}

std::string C_Globals::AutomaticNewStroke(std::string label, int window_size_x)
{
    std::string outputstring = "";
    std::string tempstring = "";
    for (int i = 0; i < label.size(); i++)
    {
        if (abs(ImGui::CalcTextSize(tempstring.c_str()).x - window_size_x) < ImGui::CalcTextSize("A").x + 1)
        {
            tempstring += "\n";
            if (label.at(i) != ' ')
                tempstring += label.at(i);

            outputstring += tempstring;
            tempstring.clear();
        }
        else
        {
            tempstring += label.at(i);
        }
    }
    
    return outputstring;
}
int C_Globals::GenerateAnimeIndexByName(std::string label)
{
    std::string index = "";

    std::string abc = Xorstr("abcdefghijklmnopqrstuvwxyz");
    
    for (int i = 0; i < label.size(); i++)
    {
        ;
    }

    return std::stoi(index);
}
int C_Globals::FindInMainArrayIndexByName(std::string label, std::vector<AnimeList> arr)
{
    int index = 0;

    for (int i = 0; i < arr.size(); i++)
    {
        if (label == arr.at(i).name)
            index = i;
    }

    return index >= 0 ? index : -1;
}

bool LoadTextureFromFile(const char* filename, PDIRECT3DTEXTURE9* out_texture)
{
    // Load texture from disk
    HRESULT hr = D3DXCreateTextureFromFileA(g_Globals->g_pd3dDevice, filename, out_texture);
    return hr == S_OK;
}

void C_Globals::ProceedAnimeListAll()
{
    if (QueueAll.empty())
        return;

    int item = QueueAll.front();
    std::string full_patch = g_Globals->AppDataPath; full_patch += Xorstr("\\AnimeHelper\\Images\\") + g_Globals->AllAnimeList.at(item).name + ".jpg";
    LoadTextureFromFile(full_patch.c_str(), &g_Globals->AllAnimeList.at(item).texture);

    QueueAll.pop_front();
}
void C_Globals::ProceedFavoriteAnime()
{
    if (QueueFavorite.empty())
        return;

    int item = QueueFavorite.front();
    std::string full_patch = g_Globals->AppDataPath; full_patch += Xorstr("\\AnimeHelper\\Images\\") + g_Globals->AnimeFavorites.at(item).name + ".jpg";
    LoadTextureFromFile(full_patch.c_str(), &g_Globals->AnimeFavorites.at(item).texture);

    QueueFavorite.pop_front();
}

void C_Globals::GetTextureAllAnimeList(int index)
{
    if (!ARRAY_SEARCH(this->QueueAll, index) && !g_Globals->AllAnimeList.at(index).texture)
        this->QueueAll.emplace_back(index);
}
void C_Globals::GetTextureFavotites(int index)
{
    if (!ARRAY_SEARCH(this->QueueFavorite, index) && !g_Globals->AnimeFavorites.at(index).texture)
        this->QueueFavorite.emplace_back(index);
}