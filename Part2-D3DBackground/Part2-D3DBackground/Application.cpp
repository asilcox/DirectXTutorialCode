#include "Application.h"

Application::Application()
	:
	window(1280, 720, "DirectX Tutorials - Direct3D Setup"),
	renderManager(window.GetHWND())
{}

Application::~Application()
{}

int Application::Run()
{
	while (true)
	{
		if (const auto msgCode = Window::ProcessMessages())
			return *msgCode;

		DrawFrame();
	}
}

void Application::DrawFrame()
{
	renderManager.ClearBuffer(0.0f, 0.1f, 0.2f);
	renderManager.EndFrame();
}

