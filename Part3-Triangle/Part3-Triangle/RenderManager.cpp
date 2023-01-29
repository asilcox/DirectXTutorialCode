#include "RenderManager.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

RenderManager::RenderManager(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferDesc.Width = 1280.0f;
	scd.BufferDesc.Height = 720.0f;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 0;
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		0, nullptr, 0, D3D11_SDK_VERSION, &scd, &pSwap, &pDevice, nullptr, &pContext);

	ID3D11Resource* pBackBuffer = nullptr;
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	if (pBackBuffer != 0)
		pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget);
	pBackBuffer->Release();

	CD3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	ID3D11DepthStencilState* pDepthSState;
	pDevice->CreateDepthStencilState(&depthDesc, &pDepthSState);

	pContext->OMSetDepthStencilState(pDepthSState, 1);

	ID3D11Texture2D* pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = 1280.0f;
	descDepth.Height = 720.0f;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthSVD = {};
	depthSVD.Format = DXGI_FORMAT_D32_FLOAT;
	depthSVD.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthSVD.Texture2D.MipSlice = 0;
	if (pDepthStencil != 0)
		pDevice->CreateDepthStencilView(pDepthStencil, &depthSVD, &pDepth);

	pContext->OMSetRenderTargets(1, &pTarget, pDepth);

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = 1280.0f;
	viewport.Height = 720.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	pContext->RSSetViewports(1, &viewport);
}

RenderManager::~RenderManager()
{
	if (pDevice) pDevice->Release();
	if (pSwap) pSwap->Release();
	if (pContext) pContext->Release();
	if (pTarget) pTarget->Release();
	if (pDepth) pDepth->Release();
}

void RenderManager::EndFrame()
{
	pSwap->Present(1, 0);
}

void RenderManager::ClearBuffer(float r, float g, float b)
{
	const float color[] = { r, g, b, 1.0f };
	pContext->ClearRenderTargetView(pTarget, color);
	pContext->ClearDepthStencilView(pDepth, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void RenderManager::DrawTriangle()
{
	std::vector<Vertex> vertices =
	{
		{ { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ { 0.0f, 0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
		{ { 0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
	};


	std::vector<unsigned short> indices =
	{
		0, 1, 2
	};

	ID3D11Buffer* pVertexBuffer;
	D3D11_BUFFER_DESC vbd = {};
	vbd.ByteWidth = UINT(sizeof(Vertex) * vertices.size());
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA vsd = {};
	vsd.pSysMem = vertices.data();
	pDevice->CreateBuffer(&vbd, &vsd, &pVertexBuffer);

	const UINT offset = 0;
	UINT stride = sizeof(Vertex);
	pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pVertexBuffer->Release();

	ID3D11Buffer* pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.ByteWidth = UINT(sizeof(unsigned short) * indices.size());
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();
	pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);

	pContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pIndexBuffer->Release();

	ID3D11VertexShader* pVertexShader;
	ID3DBlob* pVSBlob;
	D3DReadFileToBlob(L"VertexShader.cso", &pVSBlob);

	pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pVertexShader);
	
	pContext->VSSetShader(pVertexShader, nullptr, 0);
	pVertexShader->Release();

	ID3D11PixelShader* pPixelShader;
	ID3DBlob* pPSBlob;
	D3DReadFileToBlob(L"PixelShader.cso", &pPSBlob);
	
	pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pPixelShader);
	
	pContext->PSSetShader(pPixelShader, nullptr, 0);
	pPixelShader->Release();
	pPSBlob->Release();

	ID3D11InputLayout* pInputLayout;

	const std::vector<D3D11_INPUT_ELEMENT_DESC> input =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	pDevice->CreateInputLayout(
		input.data(), (UINT)input.size(),
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&pInputLayout
	);
	pVSBlob->Release();

	pContext->IASetInputLayout(pInputLayout);
	pInputLayout->Release();

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pContext->DrawIndexed((UINT)indices.size(), 0, 0);
}