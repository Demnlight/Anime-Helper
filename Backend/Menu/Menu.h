#pragma once
#include <string>
#include "../../ImGui/imgui.h"
#include "../../ImGui/imgui_impl_dx9.h"
#include "../../ImGui/imgui_impl_win32.h"
#include "../../ImGui/imgui_internal.h"
class CMenu
{
public:
	int CurrentTab = 0;
	int CurrentSubTab = 0;
	int ProgressTab = 0;
	int SameLineMax = 6;
	//int CurrentItemForSameLine = 1;
	std::string CurrentSelectedName = "";
	int CurrentSelectedIndex = 0;

	void Tabs();
	void SubTabs();

	void TabHome();
	void TabAccount();

	void SubTabLogin();
	void SubTabRegister();
	void SubTabAll();
	void SubTabMy();

	void DrawMainForm();
	void ResizeImagesByWindowSize();

	bool m_bNeedAnimate = false;

	ImVec2 FormSize = ImVec2(300, 250);

	std::string ClampName(const char* label, int max_charpters);

	ImVec2 wp = ImVec2(0, 0);
	ImVec2 ws = ImVec2(0, 0);

	ImVec2 m_vecImageSize = ImVec2(200, 320);
};

inline CMenu* g_Menu = new CMenu();