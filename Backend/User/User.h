#pragma once

class CUser
{
public:
	std::string UserName = "";
	std::string PersonalLink = "";
};

inline CUser* g_User = new CUser();