#pragma once

#include <string>

#include "Message.hpp"
#include "../../Security/xorstr.hpp"

class C_Server
{
public:
	virtual void Instance( );
	virtual bool SendNetMsg(std::string jMessage, C_Message* jAnswer );
	virtual void Register(std::string strUsername, std::string strPassword);
	virtual void Login(std::string strUsername, std::string strPassword);
private:
	int m_Socket = 0;

	bool m_bIsConnected = false;
};

inline C_Server* g_Server = new C_Server();