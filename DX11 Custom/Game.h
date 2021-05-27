#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include "Mesh.h"
#include "gameEntity.h"
#include <vector>
#include "Camera.h"
#include "Material.h"
#include "SimpleShader.h"
#include "Lights.h"
#include "PointLight.h"
#include "WICTextureLoader.h"
#include "Sky.h"
#include "DDSTextureLoader.h"
#include "SpriteFont.h"
#include "SimpleMath.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	float angle;
	float scaleSize;

	std::vector<gameEntity*> entities;
	std::vector<gameEntity*> col3;
	std::vector<gameEntity*> col2;
	std::vector<gameEntity*> col1;
	std::vector<gameEntity*> col4;
	std::vector<gameEntity*> col5;
	std::vector<gameEntity*> entityPos;
	std::vector<std::vector<gameEntity*>> grounds;
	std::vector<gameEntity*> groundGroup;
	std::vector<std::vector<gameEntity*>> allCols;

	bool doneInput = false;
	bool ready = false;

private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateBasicGeometry();

	
	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture2SRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobbleA, cobbleN, cobbleR, cobbleM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorA, floorN, floorR, floorM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> paintA, paintN, paintR, paintM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedA, scratchedN, scratchedR, scratchedM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeA, bronzeN, bronzeR, bronzeM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughA, roughN, roughR, roughM;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodA, woodN, woodR, woodM;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	// Shaders and shader-related constructs
	//Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
//	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShaderNormal;
	SimpleVertexShader* vertexShaderNormal;

	//Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Sky* skyObj;

	//camera
	Camera* cam;

	//mesh objects
	Mesh* obj1;
	Mesh* obj2;
	Mesh* obj3;
	Mesh* obj4;
	Mesh* obj5;
	Mesh* obj6;


	//game entities
	gameEntity* g1;
	gameEntity* g2;
	gameEntity* g3;
	gameEntity* player;
	gameEntity* g5;
	gameEntity* g6;
	gameEntity* ground;


	//materials
	Material* mat1;
	Material* mat2;
	Material* mat3;
	Material* mat4;
	Material* mat5;
	Material* mat6;
	Material* mat7;

	//directional light objs
	DirectionalLight light;
	DirectionalLight light2;
	DirectionalLight light3;

	PointLight point1;

	std::unique_ptr<DirectX::SpriteFont> m_font;

	DirectX::SimpleMath::Vector2 m_fontPos;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	bool playerDead;

	float score;
	float benchMark;

	float speedMult;

	float highScore;
};

