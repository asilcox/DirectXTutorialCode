#include "Application.h"

Application::Application()
	:
	window(1280, 720, "DirectX Tutorials - Point Light"),
	renderManager(window.GetHWND())
{
	char suzanne[] = "Suzanne.obj";
	meshes.push_back(renderManager.LoadOBJ(&suzanne[0]));
	char cube[] = "Cube.obj";
	meshes.push_back(renderManager.LoadOBJ(&cube[0]));
}

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
	meshes[0].translation = { 0.0f, -0.4f, 5.0f };
	meshes[0].rotation = { (11.0f * 3.14f) / 6.0f, (7.0f * 3.14f) / 6.0f, 0.0f };
	meshes[0].scale = { 1.0f, 1.0f, 1.0f };

	meshes[1].translation = { 0.0f, -2.0f, 5.0f };
	meshes[1].rotation = { 0.0f, 0.0f, 0.0f };
	meshes[1].scale = { 2.0f, 1.0f, 2.0f };

	renderManager.ClearBuffer(0.0f, 0.1f, 0.2f);
	renderManager.DrawOBJ(meshes[0]);
	renderManager.DrawOBJ(meshes[1]);
	renderManager.EndFrame();
}