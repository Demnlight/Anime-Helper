#pragma once
#include <vector>
#include <iostream>
#include <string>

#include "../../Security/xorstr.hpp"
#include <D3dx9tex.h>
#pragma comment(lib, "D3dx9")

enum RETURN : int
{
	RETURN_OK = 0,
	RETURN_NO_FILE = 1,
};

class AnimeList
{
public:
	std::string name = Xorstr("");
	std::string desc = Xorstr("");
	std::string image_url = Xorstr("");
	std::string site_url = Xorstr("");

	LPDIRECT3DTEXTURE9 texture = NULL;

	int PushAnimeList();
	void SaveAnimeList();
};
class CInternet
{
public:
	void DownloadImage(int index);
	std::string GetUrlData(std::string host, std::string url);
};

inline AnimeList* g_AnimeList = new AnimeList();
inline CInternet* g_Internet = new CInternet();