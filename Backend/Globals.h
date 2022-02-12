#pragma once

#include "AnimeList/AnimeListBox.h"
#include "User/User.h"
#include "Logger.h"

#include <d3d9.h>
#include <deque>
#include <map>
enum LEVEL_COLORS : uint32_t
{
	COLOR_NONE = 15, // black bg and white fg
	COLOR_DEBUG = 8,
	COLOR_INFO = 10,
	COLOR_WARN = 14,
	COLOR_ERROR = 12
};

class C_Globals
{
public:
	std::vector<AnimeList> AllAnimeList;
	std::vector<AnimeList> AnimeFavorites;

	std::vector<std::string> ParserAnimeListNames;
	std::vector<std::string> ParserCurrentAnimePage;
	std::vector<std::string> ParserImagesAnime;

	std::vector<std::string> AnimeByteImages;

	void Init();
	void CheckVersion();
	void FormatAnimeName(std::vector<AnimeList>* arr);
	int FindInMainArrayIndexByName(std::string label, std::vector<AnimeList> arr);
	int GenerateAnimeIndexByName(std::string label);

	std::string AutomaticNewStroke(std::string label, int window_size_x);

	void GetTextureAllAnimeList(int index);
	void GetTextureFavotites(int index);

	LPDIRECT3DDEVICE9        g_pd3dDevice;
	D3DPRESENT_PARAMETERS    g_d3dpp;
	LPDIRECT3D9              g_pD3D;

	TCHAR AppDataPath[MAX_PATH];

	float m_flScrollAmount = 0.f;
	float m_flOldScrollAmount = 0.f;

	std::deque<int> QueueAll = {};
	std::deque<int> QueueFavorite = {};

	void ProceedAnimeListAll();
	void ProceedFavoriteAnime();
};

inline C_Globals* g_Globals = new C_Globals();