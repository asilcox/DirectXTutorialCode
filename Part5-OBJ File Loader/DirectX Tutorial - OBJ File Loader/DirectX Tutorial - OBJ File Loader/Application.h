#pragma once
#include "Window.h"
#include "RenderManager.h"

class Application
{
public:
	Application();
	~Application();
	int Run();
	void DrawFrame();
private:
	Window window;
	RenderManager renderManager;

	std::vector<OBJLoaderMesh> meshes;

	std::vector<float> meshRotations;
};