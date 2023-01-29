#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>
#include "EngineMath.h"

class RenderManager
{
public:
	RenderManager(HWND hWnd);
	~RenderManager();
	void EndFrame();
	void ClearBuffer(float r, float g, float b);
	void DrawCube(float angle);
private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
	ID3D11DepthStencilView* pDepth = nullptr;
};