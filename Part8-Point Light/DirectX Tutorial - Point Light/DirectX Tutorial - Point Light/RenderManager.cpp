#include "RenderManager.h"
#include <fstream>
#include <sstream>

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

void RenderManager::DrawOBJ(OBJLoaderMesh mesh)
{
	ID3D11Buffer* pVertexBuffer;

	D3D11_BUFFER_DESC vbd = {};
	vbd.ByteWidth = UINT(sizeof(Vertex) * mesh.vertices.size());
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA vsd = {};
	vsd.pSysMem = mesh.vertices.data();
	pDevice->CreateBuffer(&vbd, &vsd, &pVertexBuffer);

	const UINT offset = 0;
	UINT stride = sizeof(Vertex);
	pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pVertexBuffer->Release();

	WorldViewProj matrices;

	DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
	world = DirectX::XMMatrixMultiply(world, DirectX::XMMatrixRotationX(mesh.rotation.x));
	world = DirectX::XMMatrixMultiply(world, DirectX::XMMatrixRotationY(mesh.rotation.y));
	world = DirectX::XMMatrixMultiply(world, DirectX::XMMatrixRotationZ(mesh.rotation.z));
	world = DirectX::XMMatrixMultiply(world, DirectX::XMMatrixScaling(mesh.scale.x, mesh.scale.y, mesh.scale.z));
	world = DirectX::XMMatrixMultiply(world, DirectX::XMMatrixTranslation(mesh.translation.x, mesh.translation.y, mesh.translation.z));
	DirectX::XMStoreFloat4x4(&matrices.wMatrix, world);

	DirectX::XMMATRIX view =
		DirectX::XMMatrixLookAtLH({ 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f });
	DirectX::XMStoreFloat4x4(&matrices.vMatrix, view);

	DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(1.0f, 1280.0f / 720.0f, 0.5f, 100.0f);
	DirectX::XMStoreFloat4x4(&matrices.pMatrix, projection);

	ID3D11Buffer* pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.ByteWidth = sizeof(matrices);
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0;
	cbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &matrices;
	pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer);

	pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	pConstantBuffer->Release();

	PointLightCBuf pLight =
	{
		{ -1.0f, 3.5f, 4.5f }, // pos
		{ 0.0f, 1.0f, 1.0f }, // diffuseColor
		7.0f, // diffuseIntensity
		30.0f, // attConst
		0.045f, // attLin
		0.075f // attQuad
	};

	ID3D11Buffer* pLightConstantBuffer;
	D3D11_BUFFER_DESC cbdLight;
	cbdLight.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbdLight.Usage = D3D11_USAGE_DYNAMIC;
	cbdLight.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbdLight.MiscFlags = 0;
	cbdLight.ByteWidth = sizeof(pLight);
	cbdLight.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA csdLight = {};
	csdLight.pSysMem = &pLight;
	pDevice->CreateBuffer(&cbdLight, &csdLight, &pLightConstantBuffer);

	pContext->PSSetConstantBuffers(0, 1, &pLightConstantBuffer);
	pLightConstantBuffer->Release();

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
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

	pContext->Draw(mesh.vertices.size(), 0);
}

OBJLoaderMesh RenderManager::LoadOBJ(char* filename)
{
	OBJLoaderMesh mesh;

	std::vector<DirectX::XMFLOAT3> vertexPos;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> texCoords;

	std::vector<DirectX::XMFLOAT3> tempVertexInfo;

	std::ifstream file(filename);

	while (!file.eof())
	{
		char line[128];
		file.getline(line, 128);

		std::stringstream s;
		s << line;

		char junk;

		if (line[0] == 'v')
		{
			if (line[1] == 'n')
			{
				DirectX::XMFLOAT3 n;
				s >> junk >> junk >> n.x >> n.y >> n.z;
				normals.push_back(n);
			}
			else if (line[1] == 't')
			{
				DirectX::XMFLOAT2 tc;
				s >> junk >> junk >> tc.x >> tc.y;
				texCoords.push_back(tc);
			}
			else
			{
				DirectX::XMFLOAT3 v;
				s >> junk >> v.x >> v.y >> v.z;
				vertexPos.push_back(v);
			}
		}

		if (line[0] == 'f')
		{
			int face[3];
			int facen[3];
			int facet[3];
			std::vector<std::string> result;
			std::string item;

			while (std::getline(s, item, ' '))
				result.push_back(item);

			std::stringstream ss[4];

			for (int i = 0; i < result.size(); ++i)
				ss[i] << result[i];

			int j = 0;
			for (int i = 1; i < 4; ++i)
			{
				int fi = 0;
				while (std::getline(ss[i], item, '/'))
				{
					if (fi == 0)
						face[j] = std::stoi(item);
					if (fi == 1)
						facet[j] = std::stoi(item);
					if (fi == 2)
						facen[j] = std::stoi(item);
					fi++;
				}
				tempVertexInfo.push_back({ (float)(face[j] - 1), (float)(facet[j] - 1) , (float)(facen[j] - 1) });
				j++;
			}
		}
	}

	std::vector<Vertex> outVertices;
	for (int j = 0; j < tempVertexInfo.size(); ++j)
	{
		Vertex v =
		{
			{ vertexPos[tempVertexInfo[j].x].x, vertexPos[tempVertexInfo[j].x].y, vertexPos[tempVertexInfo[j].x].z }, 
			{ normals[tempVertexInfo[j].z].x, normals[tempVertexInfo[j].z].y, normals[tempVertexInfo[j].z].z }, 
			{ texCoords[tempVertexInfo[j].y].x, texCoords[tempVertexInfo[j].y].y }
		};
		outVertices.push_back(v);
	}

	for (int i = 0; i < outVertices.size(); ++i)
		mesh.vertices.push_back(outVertices[i]);

	return mesh;
}