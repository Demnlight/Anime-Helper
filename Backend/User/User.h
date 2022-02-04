#pragma once
#include <string>
class CUser
{
public:
	std::string UserName = "";
	std::string Token = "";
	std::string Hwid = "";
	std::string GetHWID();
};

inline CUser* g_User = new CUser();