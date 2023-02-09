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

struct OBJLoaderMesh
{
	std::vector<Vertex> vertices;
	DirectX::XMFLOAT3 translation;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
};

struct PointLightCBuf
{
	alignas(16) DirectX::XMFLOAT3 pos;
	alignas(16) DirectX::XMFLOAT3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};