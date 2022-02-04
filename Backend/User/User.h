#pragma once
#include <string>
class CUser
{
public:
	std::string UserName = "";
	std::string Password = "";
	std::string PersonalLink = "";
	std::string AvatarLink = "";
	std::string index = "";
	//LPDIRECT3DTEXTURE9 AvatarTexture;

	void Login(std::string username, std::string password);
};

inline CUser* g_User = new CUser();