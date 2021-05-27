#include "Sky.h"

Sky::Sky(Mesh* m, ID3D11SamplerState* samp, ID3D11Device* device)
{
	//set rasterizer description
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_FRONT;
	device->CreateRasterizerState(&rastDesc, &rastState);

	//set depth stencil description
	D3D11_DEPTH_STENCIL_DESC stencilDesc = {};
	stencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&stencilDesc, &depthState);
	meshObj = m;
}

Sky::~Sky()
{
	//delete pointers
	delete simplePixel;
	delete simpleVertex;
	delete meshObj;
}

void Sky::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Camera* cam)
{
	//set rasterizer and depth stencil states
	context->RSSetState(rastState.Get());
	context->OMSetDepthStencilState(depthState.Get(), 0);

	//set shaders
	simpleVertex->SetShader();
	simplePixel->SetShader();

	//set sample state and texture values in pixel shader 
	simplePixel->SetSamplerState("sampleState", sampleOptions.Get());
	simplePixel->SetShaderResourceView("cube", shaderView.Get());

	//set view and proj matrices in vertex shader
	simpleVertex->SetMatrix4x4("view", cam->getView());
	simpleVertex->SetMatrix4x4("proj", cam->getProj());

	//copy buffer data
	simplePixel->CopyAllBufferData();
	simpleVertex->CopyAllBufferData();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	//set vertex and index buffers and draw
	context->IASetVertexBuffers(0, 1, meshObj->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(meshObj->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(
		meshObj->GetIndexCount(),
		0,
		0);

	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);

}
