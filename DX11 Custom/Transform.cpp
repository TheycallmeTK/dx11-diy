#include "Transform.h"
#include <wrl/event.h>
#include <d3d11.h>

using namespace DirectX;

Transform::Transform()
{
	
	position = DirectX::XMFLOAT3(0,0,0);
	scale = DirectX::XMFLOAT3(1,1,1);
	rotation = DirectX::XMFLOAT3(0,0,0);
	XMStoreFloat4x4(&world, DirectX::XMMatrixIdentity());
	matrixDirty = false;
}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	matrixDirty = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	rotation.x = pitch;
	rotation.y = yaw;
	rotation.z = roll;
	matrixDirty = true;

}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
	matrixDirty = true;

}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (matrixDirty) 
	{
		DirectX::XMMATRIX trans = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&position));
		DirectX::XMMATRIX rotate = DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&rotation));
		DirectX::XMMATRIX sc = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		DirectX::XMMATRIX w = sc * rotate * trans;
		DirectX::XMStoreFloat4x4(&world, w);
		matrixDirty = false;
	}
	
	return world;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	matrixDirty = true;

}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;
	matrixDirty = true;

}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
	
	matrixDirty = true;

}

void Transform::MoveRelative(float x, float y, float z)
{
	XMVECTOR vect;
	vect = XMVectorSet(x, y, z, 0);
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMVECTOR relative = XMVector3Rotate(vect, quat);
	XMStoreFloat3(&position, XMLoadFloat3(&position) + relative);

}


