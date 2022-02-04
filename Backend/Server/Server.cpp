#include <WS2tcpip.h>

#include "Server.hpp"
#include "../User/User.h"
#include "../Globals.h"

#pragma comment( lib, "ws2_32.lib" )

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

bool C_Server::SendNetMsg(std::string jMessage, C_Message* jServerAnswer)
{
	int32_t iSendedBytes = send( ( SOCKET )( m_Socket ), jMessage.c_str(), strlen(jMessage.c_str()), 0);
	if ( iSendedBytes == INVALID_SOCKET )
		return false;	

	char aRecv[ 32000 ];
	ZeroMemory( aRecv, 32000 );

	int32_t iReceived = recv( ( SOCKET )( m_Socket ), aRecv, 32000, 0 );
	if ( iReceived == INVALID_SOCKET )
		return false;

	if ( !jServerAnswer->Accept( aRecv ) )
		return false;

	if ( jServerAnswer->GetJSON( )[Xorstr( "Data" ) ][Xorstr( "Status" ) ].get< std::string >( ) != Xorstr( "0" ) )
		return false;

	return true;
}

void C_Server::Register(std::string strUsername, std::string strPassword)
{
	C_Message jAnswer;
	nlohmann::json jMessage;

	jMessage[Xorstr("Data")][Xorstr("Status")] = std::to_string(RESULT_OK);
	jMessage[Xorstr("Data")][Xorstr("Type")] = std::to_string(MSG_TYPE::MSG_REGISTER);
	jMessage[Xorstr("Data")][Xorstr("Username")] = strUsername;
	jMessage[Xorstr("Data")][Xorstr("Password")] = strPassword;

	auto string = jMessage.dump(4);

	if (!SendNetMsg(string, &jAnswer))
		return;
}

void C_Server::Login(std::string strUsername, std::string strPassword)
{
	C_Message jAnswer;
	nlohmann::json jMessage;

	jMessage[Xorstr("Data")][Xorstr("Status")] = std::to_string(RESULT_OK);
	jMessage[Xorstr("Data")][Xorstr("Type")] = std::to_string(MSG_TYPE::MSG_LOGIN);
	jMessage[Xorstr("Data")][Xorstr("Username")] = strUsername;
	jMessage[Xorstr("Data")][Xorstr("Password")] = strPassword;

	auto string = jMessage.dump(4);

	if (!SendNetMsg(string, &jAnswer))
		return;

	CUser* user = new CUser();
	user->UserName = strUsername;
	user->Password = strPassword;
	g_Globals->CurrentUser = user;
	delete user;
}