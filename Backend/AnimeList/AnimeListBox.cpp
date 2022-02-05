#include "AnimeListBox.h"
#include "../Globals.h"
#include "../../Data/json.hpp"

#include <fstream>
#include <Windows.h>
#include <iomanip>

int AnimeList::PushAnimeList()
{
	return 0;
}

void AnimeList::SaveAnimeList()
{
	nlohmann::json allJson;
	nlohmann::json& jAnimeFavoriteList = allJson[Xorstr("AnimeFavoriteList")];

	//Save AnimeFavorites
	for (int i = 0; i < g_Globals->AnimeFavorites.size(); i++)
	{
		auto it = g_Globals->AnimeFavorites.at(i);
		jAnimeFavoriteList[i][Xorstr("name")] = it.name;
		jAnimeFavoriteList[i][Xorstr("is_custom")] = it.is_custom;
		jAnimeFavoriteList[i][Xorstr("desc")] = it.desc;
		jAnimeFavoriteList[i][Xorstr("rating")] = it.rating;
		jAnimeFavoriteList[i][Xorstr("last_season_saw")] = it.last_season_saw;
		jAnimeFavoriteList[i][Xorstr("last_series_saw")] = it.last_series_saw;
		jAnimeFavoriteList[i][Xorstr("last_time_saw")] = it.last_time_saw;
		jAnimeFavoriteList[i][Xorstr("link_for_watching")] = it.link_for_watching;

		jAnimeFavoriteList[i][Xorstr("size")] = g_Globals->AnimeFavorites.size();
	}

	std::string FilePath = g_Globals->AppDataPath;
	FilePath += Xorstr("\\AnimeHelper\\");

	std::string FileName = g_Globals->AppDataPath;
	FileName += Xorstr("\\AnimeHelper\\Favorite.cfg");
	CreateDirectory(FilePath.c_str(), NULL);

	std::ofstream out_file(FileName, std::ios::out | std::ios::trunc);
	out_file << allJson.dump(4);
	out_file.close();
}