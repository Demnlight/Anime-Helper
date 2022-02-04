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
#pragma comment(lib, "D3dx9")

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

    while (g_User->UserName.length() < 2)
    {
        Sleep(500);
    }

    g_Globals->CheckVersion();
    g_Server->PushAnimeList();
    for (int i = 0; i < g_Globals->AllAnimeList.size(); i++)
        g_Internet->DownloadImage(i);
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

bool LoadTextureFromFile(const char* filename, PDIRECT3DTEXTURE9* out_texture, int* out_width, int* out_height)
{
    // Load texture from disk
    PDIRECT3DTEXTURE9 texture;
    HRESULT hr = D3DXCreateTextureFromFileA(g_Globals->g_pd3dDevice, filename, &texture);
    if (hr != S_OK)
        return false;

    // Retrieve description of the texture surface so we can access its size
    D3DSURFACE_DESC my_image_desc;
    texture->GetLevelDesc(0, &my_image_desc);
    *out_texture = texture;
    *out_width = (int)my_image_desc.Width;
    *out_height = (int)my_image_desc.Height;
    return true;
}

LPDIRECT3DTEXTURE9 C_Globals::GetTexture(std::vector<AnimeList>* arr, int index)
{
    int my_image_width = 0;
    int my_image_height = 0;
    std::string full_patch = g_Globals->AppDataPath; full_patch += Xorstr("\\AnimeHelper\\Images\\") + arr->at(index).name + ".jpg";

    if (!arr->at(index).texture)
    {
        bool ret = LoadTextureFromFile(full_patch.c_str(), &arr->at(index).texture, &my_image_width, &my_image_height);

        if (ret)
            return arr->at(index).texture;
        else
            return nullptr;
    }
    else
    {
        //std::remove(full_patch.c_str());
        return arr->at(index).texture;
    }
}