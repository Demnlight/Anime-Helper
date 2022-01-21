#include "User.h"
#include "../Globals.h"
#include "../../Security/base64.h"

void C_Globals::RegisterNewUser(std::string name, std::string link, std::string avalink)
{
	CUser user;
	user.UserName = name;
	user.PersonalLink = link;
	user.AvatarLink = avalink;
	user.index = g_Crypt->base64_encode(name.c_str());
	this->Users.push_back(user);
}

CUser* C_Globals::ParseDataForCurrentUser(std::string name)
{
	std::string CurrentUserIndex = g_Crypt->base64_decode(name.c_str());

	int UserIndexInMassive = 0;
	for (int i = 0; i < this->Users.size(); i++)
	{
		if (this->Users.at(i).index == CurrentUserIndex)
		{
			UserIndexInMassive = i;
			break;
		}
	}

	CUser* user = &this->Users.at(UserIndexInMassive);
	if (!user)
		return user;
	else
		return nullptr;
}