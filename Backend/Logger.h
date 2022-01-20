#pragma once
#include <cstdint>
#include <stdio.h>
#include <Windows.h>
#include <functional>

class CLogger
{
public:
	enum LEVEL_COLOR : uint32_t
	{
		COLOR_NONE = 15, // black bg and white fg
		COLOR_DEBUG = 8,
		COLOR_INFO = 10,
		COLOR_WARN = 14,
		COLOR_ERROR = 12
	};
	void* m_console_handle = nullptr;

	inline bool ensure_handle() {
		if (!m_console_handle)
			m_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		return static_cast<bool>(m_console_handle);
	}

	inline void SetColor(uint32_t clr) {
		if (!ensure_handle()) return;
		SetConsoleTextAttribute(m_console_handle, clr);
	}
	inline void ResetColor() {
		SetColor(static_cast<uint32_t>(COLOR_NONE));
	}

	inline void Fatal(const char* fmt, ...)
	{
		SetColor(COLOR_INFO);
		printf("[ Logger ]");
		ResetColor();

		SetColor(COLOR_ERROR);
		printf("[Error]");
		ResetColor();

		printf(" %s \n", fmt);
		
		ExitProcess(-1);
	}

	__forceinline void StartUp() {
		::AllocConsole();
		freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
		freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
		::SetConsoleTitleA("AnimeHelper");
	}

	__forceinline void Detach() {
		::ShowWindow(::GetConsoleWindow(), SW_HIDE);
		::FreeConsole();
	}
};
inline CLogger* g_Logger = new CLogger();