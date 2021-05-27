#pragma once
#include <DirectXMath.h>
#include <wrl/event.h>
#include <d3d11.h>
#include "SimpleShader.h"

class Material
{
public:
	DirectX::XMFLOAT4 colorTint;

	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	Material(DirectX::XMFLOAT4 tint, SimplePixelShader* pixShader, SimpleVertexShader* vertShader, float spec, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv, Microsoft::WRL::ComPtr<ID3D11SamplerState> sample, bool normal, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> norm, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metal, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughness );
	DirectX::XMFLOAT4 getTint();
	void setTint(DirectX::XMFLOAT4 tint);
	SimplePixelShader* getPixel();
	SimpleVertexShader* getVertex();
	float specExponent;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> getSampler();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getSRV();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughnessMap;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalMap;

	bool hasNormal;

};

