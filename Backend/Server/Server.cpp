#include <WS2tcpip.h>
#include <fstream>

#include "Server.hpp"
#include "../User/User.h"
#include "../Globals.h"

#pragma comment( lib, "ws2_32.lib" )

bool is_empty(std::ifstream& pFile)
{
	return pFile.peek() == std::ifstream::traits_type::eof();
}

void C_Server::Instance( )
{
	WSADATA WSAData;
	int32_t iResult = WSAStartup( MAKEWORD( 2, 2 ), &WSAData );
	if ( iResult == INVALID_SOCKET )
		return;
	
	m_Socket = ( int )( socket( AF_INET, SOCK_STREAM, 0 ) );
	if ( ( SOCKET )( m_Socket ) == INVALID_SOCKET )
		return;

	sockaddr_in pSocketAddress;
    pSocketAddress.sin_family = AF_INET;
    pSocketAddress.sin_port = htons( 14312 );
    inet_pton( AF_INET, Xorstr( "127.0.0.1" ), &pSocketAddress.sin_addr );

	while ( !m_bIsConnected )
	{
		int32_t iConnectionResult = connect( ( SOCKET )( m_Socket ), ( sockaddr* )( &pSocketAddress ), sizeof( sockaddr_in ) );
		if ( iConnectionResult == SOCKET_ERROR )
			continue;

		m_bIsConnected = true;
		printf("Connected \n");
	}
}

bool C_Server::SendNetMsg(std::string jMessage, std::string& ServerAnswer)
{
	int32_t iSendedBytes = send( ( SOCKET )( m_Socket ), jMessage.c_str(), strlen(jMessage.c_str()), 0);
	if ( iSendedBytes == INVALID_SOCKET )
		return false;	

	char aRecv[512000];
	ZeroMemory( aRecv, 512000);

	int32_t iReceived = recv( ( SOCKET )( m_Socket ), aRecv, 512000, 0 );
	if ( iReceived == INVALID_SOCKET )
		return false;

	ServerAnswer = aRecv;

	//if ( !jServerAnswer->Accept( aRecv ) )
	//	return false;

	//if ( jServerAnswer->GetJSON( )[Xorstr( "Data" ) ][Xorstr( "Status" ) ].get< std::string >( ) != Xorstr( "0" ) )
	//	return false;

	return true;
}

void C_Server::Register(std::string strUsername, std::string strPassword)
{
	std::string jAnswer;
	nlohmann::json jMessage;

	jMessage[Xorstr("Data")][Xorstr("Status")] = std::to_string(RESULT_OK);
	jMessage[Xorstr("Data")][Xorstr("Type")] = std::to_string(MSG_TYPE::MSG_REGISTER);
	jMessage[Xorstr("Data")][Xorstr("Username")] = strUsername;
	jMessage[Xorstr("Data")][Xorstr("Password")] = strPassword;

	auto string = jMessage.dump(4);

	if (!SendNetMsg(string, jAnswer))
		return;
}

void C_Server::Login(std::string strUsername, std::string strPassword)
{
	std::string jAnswer;
	nlohmann::json jMessage;

	jMessage[Xorstr("Data")][Xorstr("Status")] = std::to_string(RESULT_OK);
	jMessage[Xorstr("Data")][Xorstr("Type")] = std::to_string(MSG_TYPE::MSG_LOGIN);
	jMessage[Xorstr("Data")][Xorstr("Username")] = strUsername;
	jMessage[Xorstr("Data")][Xorstr("Password")] = strPassword;

	auto string = jMessage.dump(4);

	if (!SendNetMsg(string, jAnswer))
		return;

	g_User->UserName = strUsername;
}

int C_Server::PushAnimeList()
{
	std::string jAnswer;
	nlohmann::json jMessage;
	nlohmann::json allJson;

	jMessage[Xorstr("Data")][Xorstr("Status")] = std::to_string(RESULT_OK);
	jMessage[Xorstr("Data")][Xorstr("Type")] = std::to_string(MSG_TYPE::MSG_PUSHLIST);
	jMessage[Xorstr("Data")][Xorstr("Username")] = g_User->UserName;

	auto string = jMessage.dump(4);

	if (!SendNetMsg(string, jAnswer))
		return 1;

	std::string FileName = g_Globals->AppDataPath;
	FileName += Xorstr("\\AnimeHelper\\List.cfg");

	std::ofstream pNewFile(FileName, std::ios::out | std::ios::trunc);
	pNewFile.clear();
	pNewFile << jAnswer.c_str();
	pNewFile.close();

	std::ifstream ifs_final;
	ifs_final.open(FileName);
	ifs_final >> allJson;
	ifs_final.close();

	if (jAnswer.length() > 10)
	{
		nlohmann::json jAnimeList = allJson[Xorstr("AnimeList")];
		if (!jAnimeList.is_null())
		{
			int AnimeListArraySize = jAnimeList[0][Xorstr("size")].get < int >();
			for (int i = 0; i < AnimeListArraySize; i++)
			{
				AnimeList anime;
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

	//remove(Xorstr("%APPDATA%/AnimeHelper/List.cfg"));
	//RemoveDirectory(Xorstr("Settings\\"));
	return 0;
}

int C_Server::SaveAnimeList()
{
	std::string jAnswer;
	nlohmann::json jMessage;

	jMessage[Xorstr("Data")][Xorstr("Status")] = std::to_string(RESULT_OK);
	jMessage[Xorstr("Data")][Xorstr("Type")] = std::to_string(MSG_TYPE::MSG_REGISTER);

	auto string = jMessage.dump(4);

	if (!SendNetMsg(string, jAnswer))
		return 0;
}