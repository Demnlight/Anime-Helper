#include "User.h"
#include "../Globals.h"
#include "../../Security/base64.h"

void C_Globals::RegisterNewUser(std::string name, std::string link, std::string avalink, std::string password)
{
	CUser user;
	user.UserName = name;
	user.PersonalLink = link;
	user.AvatarLink = avalink;
	user.index = g_Crypt->base64_encode(name);
	user.Password = g_Crypt->base64_encode(password);
	this->Users.push_back(user);
}

void CUser::Login(std::string username, std::string password)
{
	/*
	bad code, do it on server
	unsecurity
	*/

	for (int i = 0; i < g_Globals->Users.size(); i++)
	{
		if (username == g_Globals->Users.at(i).UserName)
		{
			g_Logger->SetColor(COLOR_INFO);
			printf("[ Login ]");
			g_Logger->ResetColor();

			printf(" Username: %s | Success \n", username.c_str());

			if (password == g_Crypt->base64_decode(g_Globals->Users.at(i).Password))
			{
				g_Logger->SetColor(COLOR_INFO);
				printf("[ Login ]");
				g_Logger->ResetColor();

				printf(" Password: %s | Success \n", g_Crypt->base64_encode(password));

				g_Globals->CurrentUser = g_Globals->ParseDataForCurrentUser(username);
			}
			else
			{
				g_Logger->SetColor(COLOR_INFO);
				printf("[ Login ]");
				g_Logger->ResetColor();

				printf(" Password: %s | Failed: Incorrect Password \n", g_Crypt->base64_encode(password));
			}
		}
		else
		{
			g_Logger->SetColor(COLOR_INFO);
			printf("[ Login ]");
			g_Logger->ResetColor();

			printf(" Username: %s | Username not founded | CurrentUserName: \n", username, g_Globals->Users.at(i).UserName);
		}
	}
}

CUser* C_Globals::ParseDataForCurrentUser(std::string name)
{
	int UserIndexInMassive = 0;
	for (int i = 0; i < this->Users.size(); i++)
	{
		if (this->Users.at(i).UserName == name)
		{
			UserIndexInMassive = i;
			break;
		}
	}

	CUser* user = &this->Users.at(UserIndexInMassive);
	if (user)
		return user;
	else
		return nullptr;
}