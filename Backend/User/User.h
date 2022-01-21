#pragma once
#include <string>
class CUser
{
public:
	std::string UserName = "";
	std::string PersonalLink = "";
	std::string AvatarLink = "";
	std::string index = "";
	LPDIRECT3DTEXTURE9 AvatarTexture;
};

inline CUser* g_User = new CUser();