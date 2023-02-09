#include "Application.h"

Application::Application()
	:
	window(1280, 720, "DirectX Tutorials - OBJ File Loader"),
	renderManager(window.GetHWND())
{
	char filename[] = "Suzanne.obj";
	meshes.push_back(renderManager.LoadOBJ(&filename[0]));
	meshes.push_back(renderManager.LoadOBJ(&filename[0]));

	meshRotations.push_back(0.0f);
	meshRotations.push_back(0.0f);
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
	meshRotations[0] += 0.01f;
	meshRotations[1] -= 0.01f;

	meshes[0].translation = { -2.0f, 0.0f, 5.0f };
	meshes[0].rotation = { meshRotations[0], meshRotations[0], meshRotations[0] };
	meshes[0].scale = { 1.0f, 1.0f, 1.0f };

	meshes[1].translation = { 2.0f, 0.0f, 5.0f };
	meshes[1].rotation = { meshRotations[1], meshRotations[1], meshRotations[1] };
	meshes[1].scale = { 0.5f, 0.5f, 0.5f };

	renderManager.ClearBuffer(0.0f, 0.1f, 0.2f);
	renderManager.DrawOBJ(meshes[0]);
	renderManager.DrawOBJ(meshes[1]);
	renderManager.EndFrame();
}