#include "Application.h"

Application::Application()
	:
	window(1280, 720, "DirectX Tutorials - Directional Light"),
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
	cubeRot += 0.01f;

	renderManager.ClearBuffer(0.0f, 0.1f, 0.2f);
	renderManager.DrawCube(cubeRot);
	renderManager.EndFrame();
}