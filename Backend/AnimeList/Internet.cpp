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