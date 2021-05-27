#pragma once
#include <DirectXMath.h>
#include <wrl/event.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "DXCore.h"


class Transform
{
public:
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotation;

	bool matrixDirty;

	Transform();

	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(float x, float y, float z);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();

	void MoveAbsolute(float x, float y, float z);
	void Rotate(float pitch, float yaw, float roll);
	void Scale(float x, float y, float z);

	void MoveRelative(float x, float y, float z);
	
};

