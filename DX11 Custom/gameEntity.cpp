#include "gameEntity.h"
#include "bufferStructs.h"
#include "Material.h"
#include "Camera.h"

gameEntity::gameEntity(Mesh* obj, Material* material, bool isStationary) 
{
	//initialize mesh and transform objects
	meshObj = obj;
	tObj = Transform();
	this->mat = material;
	stationary = isStationary;
	if (!isStationary) {
		isActive = false;
	}
	
	
}

gameEntity::~gameEntity()
{
	
	
}

Mesh* gameEntity::GetMesh()
{
	return meshObj;
}

Transform* gameEntity::GetTransform()
{
	return &tObj;
}

//draw function called in draw/game.cpp
void gameEntity::draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, UINT stride, UINT offset, Camera* cam)
{
	//setting shaders from material with simpleshader

	mat->getVertex()->SetShader();
	mat->getPixel()->SetShader();

	//set srv and sampler in pixel shader
	mat->getPixel()->SetShaderResourceView("Albedo", mat->getSRV().Get());
	mat->getPixel()->SetSamplerState("samplerOptions", mat->getSampler().Get());

	//if texture has a normal, set normal map in pixel shader
	if (mat->hasNormal) {
		mat->getPixel()->SetShaderResourceView("NormalMap", mat->normalMap.Get());
	}

	mat->getPixel()->SetShaderResourceView("RoughnessMap", mat->roughnessMap.Get());
	mat->getPixel()->SetShaderResourceView("MetalnessMap", mat->metalMap.Get());



	//set the values of the vertex shader 
	SimpleVertexShader* vs = mat->getVertex(); 
	vs->SetFloat4("colorTint", mat->getTint());
	vs->SetMatrix4x4("world", tObj.GetWorldMatrix());
	vs->SetMatrix4x4("view", cam->getView());
	vs->SetMatrix4x4("proj", cam->getProj());

	//copy buffer data
	vs->CopyAllBufferData();
	
	//set vertex and index buffers
	context->IASetVertexBuffers(0, 1, meshObj->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(meshObj->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	//draw entity
	context->DrawIndexed(
		meshObj->GetIndexCount(),     
		0,    
		0);
	
}
