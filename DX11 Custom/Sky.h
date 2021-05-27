#pragma once
#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h>
#include "Mesh.h"
#include "SimpleShader.h"
#include "Camera.h"

class Sky
{
public:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampleOptions;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;
	Mesh* meshObj;
	SimplePixelShader* simplePixel;
	SimpleVertexShader* simpleVertex;


	Sky(Mesh* m, ID3D11SamplerState* samp, ID3D11Device* device);
	~Sky();

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Camera* cam);
};

