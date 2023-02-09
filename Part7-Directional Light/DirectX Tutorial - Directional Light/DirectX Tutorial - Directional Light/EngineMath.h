#pragma once
#include <DirectXMath.h>

struct WorldViewProj
{
	DirectX::XMFLOAT4X4 wMatrix;
	DirectX::XMFLOAT4X4 vMatrix;
	DirectX::XMFLOAT4X4 pMatrix;
};

struct Vertex
{
	struct
	{
		float x;
		float y;
		float z;
	} pos;

	struct
	{
		float x;
		float y;
		float z;
	} normal;

	struct
	{
		float u;
		float v;
	} texCoord;
};

struct DirectionalLightCBuf
{
	alignas(16) DirectX::XMFLOAT3 dir;
	alignas(16) DirectX::XMFLOAT3 pos;
	alignas(16) DirectX::XMFLOAT3 ambient;
	alignas(16) DirectX::XMFLOAT3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};