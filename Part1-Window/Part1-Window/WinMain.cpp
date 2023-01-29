#include "Window.h"

int CALLBACK WinMain(
	_In_	 HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_	 LPSTR	   lpCmdLine,
	_In_	 int	   nShowCmd)
{
	Window window(1280, 720, "DirectX Tutorials - Window");

	while (true)
	{
		if (const auto msgCode = Window::ProcessMessages())
			return *msgCode;
	}
}