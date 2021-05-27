#pragma once
#include <DirectXMath.h>
#include <wrl/event.h>
#include <d3d11.h>

struct VertexShaderExternalData 
{ 
	DirectX::XMFLOAT4 colorTint; 
	//DirectX::XMFLOAT3 offset;
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;
};

//constant buffer


