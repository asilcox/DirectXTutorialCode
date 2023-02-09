#pragma once
#include <Windows.h>
#include <optional>

class Window
{
public:
	Window(int width, int height, const char* name);
	~Window();
	static std::optional<int> ProcessMessages();
	HWND GetHWND();
private:
	static LRESULT __stdcall WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	int width;
	int height;
	HWND hWnd;
	const char* windowName = "DirectX Tutorials";
	HINSTANCE hInstance;
};