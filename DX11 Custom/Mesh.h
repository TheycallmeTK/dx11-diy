#pragma once
#include <DirectXMath.h>
#include <wrl/event.h>
#include <d3d11.h>
#include "Vertex.h"
#include <fstream>


class Mesh
{
public:
	Mesh(const char* file, Microsoft::WRL::ComPtr<ID3D11Device> d3Device);
	Mesh(Vertex v[], int verts, unsigned int inds[], int numInds, Microsoft::WRL::ComPtr<ID3D11Device> d3Device);
	~Mesh();
	//vertex and index buffers
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	int indices;

	//vertext index buffer get functions
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	void createBuffers(Vertex v[], int verts, unsigned int inds[], int numInds, Microsoft::WRL::ComPtr<ID3D11Device> d3Device);
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
};

