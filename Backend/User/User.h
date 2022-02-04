#pragma once
#include <string>
class CUser
{
public:
	std::string UserName = "";
};

inline CUser* g_User = new CUser();