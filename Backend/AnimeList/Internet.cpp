#include "AnimeListBox.h"
#include "../Globals.h"

#include <string>
#include <wininet.h>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <thread>
#pragma comment (lib, "wininet.lib")
#pragma comment (lib, "urlmon.lib")

#pragma warning(disable : 4996)

std::string CInternet::GetUrlData(std::string host, std::string url)
{
	std::string data("");

	auto internet = InternetOpenA("", INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0u);

	if (!internet)
		return data;

	auto session = InternetConnectA(internet, host.c_str(), 80, 0, 0, INTERNET_SERVICE_HTTP, 0u, 0u);

	if (!session)
		return data;

	auto request = HttpOpenRequestA(session, Xorstr("POST"), url.c_str(), 0, 0, 0, INTERNET_FLAG_RELOAD, 0u);

	if (!session)
		return data;

	std::string headers(Xorstr("Content-Type: application/x-www-form-urlencoded"));

	char buf[4096];

	if (!HttpSendRequestA(request, headers.data(), strlen(headers.data()), buf, strlen(buf)))
		return data;

	char buffer[4096];

	DWORD read = 0u;

	while (InternetReadFile(request, buffer, sizeof(buffer) - 1, &read) && read)
		data.append(buffer, read);

	InternetCloseHandle(session);
	InternetCloseHandle(request);
	InternetCloseHandle(session);

	return data;
}

void FormatName(int index)
{
	for (int i = 0; i < g_Globals->AllAnimeList.at(index).name.length(); i++)
		if (g_Globals->AllAnimeList.at(index).name.at(i) == '?' || g_Globals->AllAnimeList.at(index).name.at(i) == ':' || g_Globals->AllAnimeList.at(index).name.at(i) == '`' || g_Globals->AllAnimeList.at(index).name.at(i) == '"' || g_Globals->AllAnimeList.at(index).name.at(i) == '/')
			g_Globals->AllAnimeList.at(index).name.at(i) = ' ';
}

void CInternet::DownloadImage(int index)
{
	
	std::string outputname = g_Globals->AppDataPath;
	outputname += Xorstr("\\AnimeHelper\\Images\\");
	outputname += g_Globals->AllAnimeList.at(index).name;
	outputname += Xorstr(".jpg");

	std::ifstream ifs_final(outputname);
	if (ifs_final)
	{
		g_Logger->SetColor(COLOR_INFO);
		printf("[ DownloadImage ]");
		g_Logger->ResetColor();

		printf(" Image with OutputName: %s Found \n", g_Globals->AllAnimeList.at(index).image_url.c_str(), outputname.c_str());
		ifs_final.close();

		return;
	}
	ifs_final.close();

	if (URLDownloadToFile(NULL, g_Globals->AllAnimeList.at(index).image_url.c_str(), outputname.c_str(), 0, NULL) == S_OK)
	{
		g_Logger->SetColor(COLOR_INFO);
		printf("[ DownloadImage ]");
		g_Logger->ResetColor();

		printf(" ImageUrl: %s | OutputName: %s \n", g_Globals->AllAnimeList.at(index).image_url.c_str(), outputname.c_str());
	}
	else
	{
		g_Logger->SetColor(COLOR_INFO);
		printf("[ DownloadImage ]");
		g_Logger->ResetColor();

		g_Logger->SetColor(COLOR_ERROR);
		printf("[ Error ]");
		g_Logger->ResetColor();

		printf(" Download Failed | ImageUrl: %s | OutputName: %s \n", g_Globals->AllAnimeList.at(index).image_url.c_str(), outputname.c_str());
	}
}

bool IsArrayHaveElementWithThisName(std::string name)
{
	for (int m = 0; m < g_Globals->ParserCurrentAnimePage.size(); m++)
	{
		for (int i = m + 1; i < g_Globals->ParserCurrentAnimePage.size(); i++)
		{
			if (g_Globals->ParserCurrentAnimePage.at(i) == name)
				return true;
		}
	}
	return false;
}

void CInternet::ParseAnimeListAndImages()
{
	std::string page = GetUrlData(Xorstr("animehelper.000webhostapp.com"), Xorstr(""));

	static int m = 0;
	for (int i = 0; i < 100; i++)
	{
		int n = page.find("h5 font-weight-normal mb-1", m);
		if (n != std::string::npos)
		{
			m = page.find("anime/", n);
			if (m != std::string::npos)
			{
				std::string imageurl;
				for (int g = m + 6; page.at(g) != '"'; g++)
				{
					imageurl += page.at(g);
				}
				imageurl.erase(imageurl.end());

				if (!IsArrayHaveElementWithThisName(imageurl))
					g_Globals->ParserCurrentAnimePage.push_back(imageurl);

				g_Logger->SetColor(COLOR_INFO);
				printf("[ Parser ]");
				g_Logger->ResetColor();

				printf(" [ ParserCurrentAnimePage ] %s \n", imageurl.c_str());

				m += 1;
			}
		}
		static int a = 0;
		int s = page.find("text-gray-dark-6 small mb-2", a);
		if (s != std::string::npos)
		{
			a = page.find("<div>", s);
			if (a != std::string::npos)
			{
				std::string nameurl;
				for (int g = a + 5; page.at(g) != '<'; g++)
				{
					nameurl += page.at(g);
				}
				nameurl.erase(nameurl.end());

				for (int i = 0; i < nameurl.length(); i++)
					if (nameurl.at(i) == '?' || nameurl.at(i) == ':')
						nameurl.erase(nameurl.begin() + i);

				if (!IsArrayHaveElementWithThisName(nameurl))
					g_Globals->ParserAnimeListNames.push_back(nameurl);

				g_Logger->SetColor(COLOR_INFO);
				printf("[ Parser ]");
				g_Logger->ResetColor();

				printf(" [ ParserAnimeListNames ]   %s \n", nameurl.c_str());

				a += 1;
			}
		}
	}
	printf("[ Parser ] Final parse page \n");

	//find image url for anime
	for (int i = 0; i < g_Globals->ParserAnimeListNames.size(); i++)
	{
		std::string url;
		url += "anime/";
		url += g_Globals->ParserCurrentAnimePage.at(i);

		std::string sitepage = GetUrlData("animego.org", url.c_str());

		int n = sitepage.find("https://animego.org/media");
		if (n != std::string::npos)
		{
			std::string imageurl;
			for (int g = n; sitepage.at(g) != '"'; g++)
			{
				imageurl += sitepage.at(g);
			}
			imageurl.erase(imageurl.end());

			if (!IsArrayHaveElementWithThisName(imageurl))
			{
				g_Globals->ParserImagesAnime.push_back(imageurl);
				printf("[ Parser ][ ImageUrl ][Iter: %i] %s | %s \n", i, imageurl.c_str(), g_Globals->ParserAnimeListNames.at(i).c_str());

			}
			else
			{
				printf("[ Parser ][ ImageUrl ][Iter: %i] IsArrayHaveElementWithThisName:true \n");
			}
		}
		else
		{
			printf("[ Parser ][ ImageUrl ][Iter: %i][Error] n = %i | AnimeName: %s | AnimeUrl: %s \n", i, n, g_Globals->ParserAnimeListNames.at(i).c_str(), g_Globals->ParserCurrentAnimePage.at(i).c_str());
		}
		Sleep(200);
	}

	printf("[ Parser ] Final parse images | arraysize: %i \n", g_Globals->ParserImagesAnime.size());

	//setup anime list
	for (int i = 0; i < g_Globals->ParserAnimeListNames.size(); i++)
	{
		AnimeList anime;
		anime.name = g_Globals->ParserAnimeListNames.at(i);
		anime.site_url = g_Globals->ParserCurrentAnimePage.at(i);
		anime.image_url = g_Globals->ParserImagesAnime.at(i);
		g_Globals->AllAnimeList.push_back(anime);

		g_Logger->SetColor(COLOR_INFO);
		printf("[ Parser ]");
		g_Logger->ResetColor();

		printf(" [ AllAnimeList ] %s \n", anime.name.c_str());
	}
}

void CInternet::ParseDescription()
{
	for (int i = 0; i < g_Globals->ParserAnimeListNames.size(); i++)
	{
		std::string url = "anime/" + g_Globals->ParserCurrentAnimePage.at(i);
		std::string sitepage = GetUrlData("animego.org", url.c_str());
		//+anime.site_url	"vayolet-evergarden-vechnost-i-prizrak-pera-1257"	std::string

		int n = sitepage.find("description pb-3");
		if (n != std::string::npos)
		{
			std::string parsed;
			for (int g = n + 18; sitepage.at(g) != '"'; g++)
			{
				parsed += sitepage.at(g);
			}
			parsed.erase(parsed.end());
			g_Globals->AllAnimeList.at(i).desc = parsed;
		}
	}
}