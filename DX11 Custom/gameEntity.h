#pragma once
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Camera.h"
class gameEntity
{
public:
	Transform tObj;
	Mesh* meshObj;
	gameEntity(Mesh* obj, Material* material, bool isStationary);
	~gameEntity();
	bool stationary;
	Mesh* GetMesh();
	Transform* GetTransform();
	
	bool isActive;
	
	void draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, UINT stide, UINT offset, Camera* cam);

	Material* mat;
};

