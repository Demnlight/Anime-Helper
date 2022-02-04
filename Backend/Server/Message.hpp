#pragma once

#include "Json.hpp"
#include "../../Security/xorstr.hpp"

enum Results
{
	RESULT_OK = 0,
	RESULT_FAIL = 1
};

enum MSG_TYPE
{
	MSG_HELLO = 0,
	MSG_REGISTER = 1,
	MSG_LOGIN = 2
};

class C_Message
{
public:
	C_Message( Results iResult = RESULT_OK, MSG_TYPE iType = MSG_HELLO, std::string strText = "" )
	{
		jMessage[Xorstr( "Data" ) ][Xorstr( "Status" ) ] = std::to_string( iResult );
		jMessage[Xorstr( "Data" ) ][Xorstr( "Type" ) ] = std::to_string( iType );
		jMessage[Xorstr( "Data" ) ][Xorstr( "Text" ) ] = strText;
	}
	
	C_Message( Results iResult, std::string strText )
	{
		jMessage[Xorstr( "Data" ) ][Xorstr( "Status" ) ] = std::to_string( iResult );
		jMessage[Xorstr( "Data" ) ][Xorstr( "Type" ) ] = Xorstr( "0" );
		jMessage[Xorstr( "Data" ) ][Xorstr( "Text" ) ] = strText;
	}
	
	int32_t GetMessageType( )
	{
		return jMessage[Xorstr( "Data" ) ][Xorstr( "Type" ) ].get< int32_t >( );
	}

	nlohmann::json& GetJSON( )
	{
		return jMessage;
	}

	bool Accept( char* cData )
	{
		if ( !jMessage.accept( cData ) )
			return false;

		jMessage = jMessage.parse( cData );
		return true;
	} 

	const char* DumpMsg( )
	{
		return jMessage.dump( 4 ).c_str( );
	}

	int32_t GetMsgLength( )
	{
		return strlen( DumpMsg( ) );
	}

private:
	nlohmann::json jMessage;
};