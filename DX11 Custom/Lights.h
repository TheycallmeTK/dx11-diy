#pragma once
#include <DirectXMath.h>
#include <wrl/event.h>
#include <d3d11.h>
//struct defining a directional light
struct DirectionalLight
{
	DirectX::XMFLOAT3 ambientColor;
	float padding1;
	DirectX::XMFLOAT3 diffuseColor;
	float padding2;
	DirectX::XMFLOAT3 direction;

};