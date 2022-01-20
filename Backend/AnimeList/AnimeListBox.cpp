#include "AnimeListBox.h"
#include "../Globals.h"
#include "../../Data/json.hpp"

#include <fstream>
#include <Windows.h>
#include <iomanip>

using json = nlohmann::json;

bool is_empty(std::ifstream& pFile)
{
	return pFile.peek() == std::ifstream::traits_type::eof();
}

int AnimeList::PushAnimeList()
{
	json allJson;

	CreateDirectory(Xorstr("Settings\\"), NULL);
	/*std::ifstream ifs;
	while (true)
	{
		ifs.open(Xorstr("Settings\\List.cfg"));
		if (!ifs)
			URLDownloadToFile(NULL, Xorstr("https://animehelper.000webhostapp.com/List.cfg"), Xorstr("Settings\\List.cfg"), 0, NULL);
		else
			break;
	}*/

	std::ifstream ifs_final;
	ifs_final.open(Xorstr("Settings\\List.cfg"));

	if (is_empty(ifs_final))
	{
		ifs_final.close();
		return RETURN_NO_FILE;
	}

	ifs_final >> allJson;
	ifs_final.close();

	/*nlohmann::json jAnimeFinished = allJson[Xorstr("AnimeFinished")];
	nlohmann::json jAnimeWatching = allJson[Xorstr("AnimeWatching")];
	nlohmann::json jAnimeWannaStart = allJson[Xorstr("AnimeWannaStart")];*/
	nlohmann::json jAnimeList = allJson[Xorstr("AnimeList")];
	nlohmann::json jAnimeFavoriteList = allJson[Xorstr("AnimeFavoriteList")];

	if (!jAnimeFavoriteList.is_null())
	{
		int AnimeFavoriteListArraySize = jAnimeFavoriteList[0][Xorstr("size")].get < int >();
		for (int i = 0; i < AnimeFavoriteListArraySize; i++)
		{
			AnimeList anime;
			//anime.index = jAnimeFavoriteList[i][Xorstr("index")].get < int >();
			anime.name = jAnimeFavoriteList[i][Xorstr("name")].get < std::string >();
			anime.desc = jAnimeFavoriteList[i][Xorstr("desc")].get < std::string >();
			anime.is_custom = jAnimeFavoriteList[i][Xorstr("is_custom")].get < bool >();
			anime.rating = jAnimeFavoriteList[i][Xorstr("rating")].get < int >();
			anime.last_season_saw = jAnimeFavoriteList[i][Xorstr("last_season_saw")].get < std::string >();
			anime.last_series_saw = jAnimeFavoriteList[i][Xorstr("last_series_saw")].get < std::string >();
			anime.last_time_saw = jAnimeFavoriteList[i][Xorstr("last_time_saw")].get < std::string >();
			anime.link_for_watching = jAnimeFavoriteList[i][Xorstr("link_for_watching")].get < std::string >();

			g_Globals->AnimeFavorites.push_back(anime);
		}
	}
	/*if (!jAnimeFinished.is_null())
	{
		int AnimeFinishedArraySize = jAnimeFinished[0][Xorstr("size")].get < int >();
		for (int i = 0; i < AnimeFinishedArraySize; i++)
		{
			AnimeList anime;
			//anime.index = jAnimeFinished[i][Xorstr("index")].get < int >();
			anime.name = jAnimeFinished[i][Xorstr("name")].get < std::string >();

			g_Globals->anime_list_finished.push_back(anime);
		}
	}
	if (!jAnimeWatching.is_null())
	{
		int AnimeWatchingArraySize = jAnimeWatching[0][Xorstr("size")].get < int >();
		for (int i = 0; i < AnimeWatchingArraySize; i++)
		{
			AnimeList anime;
			//anime.index = jAnimeWatching[i][Xorstr("index")].get < int >();
			anime.name = jAnimeWatching[i][Xorstr("name")].get < std::string >();

			g_Globals->anime_list_watching.push_back(anime);
		}
	}
	if (!jAnimeWannaStart.is_null())
	{
		int AnimeWannaStartArraySize = jAnimeWannaStart[0][Xorstr("size")].get < int >();
		for (int i = 0; i < AnimeWannaStartArraySize; i++)
		{
			AnimeList anime;
			//anime.index = jAnimeWannaStart[i][Xorstr("index")].get < int >();
			anime.name = jAnimeWannaStart[i][Xorstr("name")].get < std::string >();

			g_Globals->anime_list_wannastart.push_back(anime);
		}
	}*/
	if (!jAnimeList.is_null())
	{
		int AnimeListArraySize = jAnimeList[0][Xorstr("size")].get < int >();
		for (int i = 0; i < AnimeListArraySize; i++)
		{
			AnimeList anime;
			//anime.index = jAnimeList[i][Xorstr("index")].get < int >();
			anime.name = jAnimeList[i][Xorstr("name")].get < std::string >();
			anime.desc = jAnimeList[i][Xorstr("desc")].get < std::string >();
			anime.image_url = jAnimeList[i][Xorstr("image_url")].get < std::string >();
			anime.rating = jAnimeList[i][Xorstr("rating")].get < int >();
			anime.status = jAnimeList[i][Xorstr("status")].get < std::string >();
			anime.genre = jAnimeList[i][Xorstr("genre")].get < std::string >();
			anime.type = jAnimeList[i][Xorstr("type")].get < std::string >();
			anime.original_source = jAnimeList[i][Xorstr("original_source")].get < std::string >();
			anime.last_season_date_release = jAnimeList[i][Xorstr("last_season_date_release")].get < std::string >();
			anime.start_and_end_date = jAnimeList[i][Xorstr("start_and_end_date")].get < std::string >();
			anime.studio = jAnimeList[i][Xorstr("studio")].get < std::string >();
			anime.MPAA_rating = jAnimeList[i][Xorstr("MPAA_rating")].get < std::string >();
			anime.age_restrictions = jAnimeList[i][Xorstr("age_restrictions")].get < std::string >();
			anime.average_part_duration = jAnimeList[i][Xorstr("average_part_duration")].get < std::string >();
			anime.ranobe_manga = jAnimeList[i][Xorstr("ranobe_manga")].get < std::string >();

			g_Globals->AllAnimeList.push_back(anime);
		}
	}
}

void AnimeList::SaveAnimeList()
{
	json allJson;
	/*nlohmann::json& jAnimeFinished = allJson[Xorstr("AnimeFinished")];
	nlohmann::json& jAnimeWatching = allJson[Xorstr("AnimeWatching")];
	nlohmann::json& jAnimeWannaStart = allJson[Xorstr("AnimeWannaStart")];*/
	nlohmann::json& jAnimeList = allJson[Xorstr("AnimeList")];
	nlohmann::json& jAnimeFavoriteList = allJson[Xorstr("AnimeFavoriteList")];

	//Save AnimeFavorites
	for (int i = 0; i < g_Globals->AnimeFavorites.size(); i++)
	{
		auto it = g_Globals->AnimeFavorites.at(i);
		//jAnimeFavoriteList[i][Xorstr("index")] = it.index;
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

	//Save anime_list_finished
	/*for (int i = 0; i < g_Globals->anime_list_finished.size(); i++)
	{
		auto it = g_Globals->anime_list_finished.at(i);
		//jAnimeFinished[i][Xorstr("index")] = it.index;
		jAnimeFinished[i][Xorstr("name")] = it.name;
		jAnimeFinished[i][Xorstr("size")] = g_Globals->anime_list_finished.size();
	}

	//Save anime_list_watching
	for (int i = 0; i < g_Globals->anime_list_watching.size(); i++)
	{
		auto it = g_Globals->anime_list_watching.at(i);
		//jAnimeWatching[i][Xorstr("index")] = it.index;
		jAnimeWatching[i][Xorstr("name")] = it.name;
		jAnimeWatching[i][Xorstr("size")] = g_Globals->anime_list_watching.size();
	}

	//Save anime_list_wannastart
	for (int i = 0; i < g_Globals->anime_list_wannastart.size(); i++)
	{
		auto it = g_Globals->anime_list_wannastart.at(i);
		//jAnimeWannaStart[i][Xorstr("index")] = it.index;
		jAnimeWannaStart[i][Xorstr("name")] = it.name;
		jAnimeWannaStart[i][Xorstr("size")] = g_Globals->anime_list_wannastart.size();
	}*/

	//Save AllAnimeList
	for (int i = 0; i < g_Globals->AllAnimeList.size(); i++)
	{
		auto it = g_Globals->AllAnimeList.at(i);
		//jAnimeList[i][Xorstr("index")] = it.index;
		jAnimeList[i][Xorstr("name")] = it.name;
		jAnimeList[i][Xorstr("desc")] = it.desc;
		jAnimeList[i][Xorstr("image_url")] = it.image_url;
		jAnimeList[i][Xorstr("rating")] = it.rating;
		jAnimeList[i][Xorstr("type")] = it.type;
		jAnimeList[i][Xorstr("status")] = it.status;
		jAnimeList[i][Xorstr("genre")] = it.genre;
		jAnimeList[i][Xorstr("original_source")] = it.original_source;
		jAnimeList[i][Xorstr("last_season_date_release")] = it.last_season_date_release;
		jAnimeList[i][Xorstr("start_and_end_date")] = it.start_and_end_date;
		jAnimeList[i][Xorstr("studio")] = it.studio;
		jAnimeList[i][Xorstr("MPAA_rating")] = it.MPAA_rating;
		jAnimeList[i][Xorstr("age_restrictions")] = it.age_restrictions;
		jAnimeList[i][Xorstr("average_part_duration")] = it.average_part_duration;
		jAnimeList[i][Xorstr("ranobe_manga")] = it.ranobe_manga;
		jAnimeList[i][Xorstr("size")] = g_Globals->AllAnimeList.size();
	}

	std::string folder, file;
	auto get_dir = [&folder, &file]() ->void
	{
		folder = Xorstr("Settings\\");
		file = Xorstr("Settings\\List.cfg");
		CreateDirectory(folder.c_str(), NULL);
	};
	get_dir();

	std::ofstream out_file(file, std::ios::out | std::ios::trunc);
	out_file << allJson.dump(4);
	out_file.close();
}