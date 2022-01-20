#include "Globals.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx9.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_internal.h"

#include <fstream>
#include <algorithm>
#include <D3dx9tex.h>

#pragma comment(lib, "D3dx9")

void C_Globals::Init()
{

    g_Globals->CheckVersion();

    g_Logger->SetColor(COLOR_DEBUG);
    printf("[ Logger ]");
    g_Logger->ResetColor();

    printf(" Start PushAnimeList \n");
    if (g_AnimeList->PushAnimeList() == RETURN_NO_FILE)
    {
        g_Logger->SetColor(COLOR_DEBUG);
        printf("[ Logger ]");
        g_Logger->ResetColor();

        printf(" Start ParseAnimeListAndImages \n");
        g_Internet->ParseAnimeListAndImages();

        g_Logger->SetColor(COLOR_DEBUG);
        printf("[ Logger ]");
        g_Logger->ResetColor();

        printf(" Start SaveAnimeList \n");
        g_AnimeList->SaveAnimeList();
    }
    //clear array from repetitions
    /*{
        for (int m = 0; m < g_Globals->AllAnimeList.size(); m++)
        {
            for (int i = m + 1; i < g_Globals->AllAnimeList.size(); i++)
            {
                if (g_Globals->AllAnimeList.at(m).name == g_Globals->AllAnimeList.at(i).name)
                {
                    g_Globals->AllAnimeList.erase(g_Globals->AllAnimeList.begin() + i);
                }
            }
        }
    }*/

    g_Logger->SetColor(COLOR_DEBUG);
    printf("[ Logger ]");
    g_Logger->ResetColor();

    printf(" Start DownloadImage \n");
    for (int i = 0; i < g_Globals->AllAnimeList.size(); i++)
        g_Internet->DownloadImage(i);
}

void C_Globals::CheckVersion()
{
    auto loader_version = g_Internet->GetUrlData(Xorstr("animehelper.000webhostapp.com"), Xorstr("version.php"));
    std::string version = Xorstr("1.002");

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

LPDIRECT3DTEXTURE9 C_Globals::GetTexture(int index)
{
    int my_image_width = 0;
    int my_image_height = 0;
    if (!g_Globals->AllAnimeList.at(index).texture)
    {
        std::string full_patch = "Settings/" + g_Globals->AllAnimeList.at(index).name + ".jpg";
        bool ret = LoadTextureFromFile(full_patch.c_str(), &g_Globals->AllAnimeList.at(index).texture, &my_image_width, &my_image_height);

        if (ret)
            return g_Globals->AllAnimeList.at(index).texture;
        else
            return nullptr;
    }
    else
        return g_Globals->AllAnimeList.at(index).texture;
}