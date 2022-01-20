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
	//int index = 0;
	std::string name = Xorstr("");
	std::string desc = Xorstr("");
	std::string image_url = Xorstr("");
	std::string site_url = Xorstr("");
	std::string type = Xorstr("TV Serial");
	int max_episodes = 0;
	std::string status = Xorstr("");
	std::string genre = Xorstr("");
	std::string original_source = Xorstr("");
	std::string last_season_date_release = Xorstr("");
	std::string start_and_end_date = Xorstr("");
	std::string studio = Xorstr("");
	std::string MPAA_rating = Xorstr("");
	std::string age_restrictions = Xorstr("");
	std::string average_part_duration = Xorstr("");
	std::string ranobe_manga = Xorstr("");


	std::string last_season_saw = Xorstr("");
	std::string last_series_saw = Xorstr("");
	std::string last_time_saw = Xorstr("");
	std::string link_for_watching = Xorstr("");


	bool is_custom = false;
	PDIRECT3DTEXTURE9 texture = NULL;
	int rating = 0;

	int PushAnimeList();
	void SaveAnimeList();
	void ParseFromSite(std::string filename);
};
class CInternet
{
public:
	void DownloadImage(int index);
	void ParseAnimeListAndImages();
	void ParseDescription();
	std::string GetUrlData(std::string host, std::string url);
};

inline AnimeList* g_AnimeList = new AnimeList();
inline CInternet* g_Internet = new CInternet();