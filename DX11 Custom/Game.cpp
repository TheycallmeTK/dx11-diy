#include "Game.h"
#include "Vertex.h"

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include "bufferStructs.h"
#include <cmath>
#include "Camera.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <d3d11.h>
// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

	ready = false;
	highScore = 0;
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Note: Since we're using smart pointers (ComPtr),
	// we don't need to explicitly clean up those DirectX objects
	// - If we weren't using smart pointers, we'd need
	//   to call Release() on each DirectX object created in Game
	delete obj1;
	delete obj2;
	delete obj3;
	delete obj4; 
	delete obj5;
	delete obj6;

	delete g1;
	delete g2;
	delete g3;
	delete player;
	delete g5;
	delete g6;
	delete ground;

	delete mat1;
	delete mat2;
	delete mat3;
	delete mat4;
	delete mat5;
	delete mat6;
	delete mat7;

	delete cam;

	delete vertexShader;
	delete pixelShader;

	delete vertexShaderNormal;
	delete pixelShaderNormal;

	delete skyObj;

	m_font.reset();
	m_spriteBatch.reset();


	entities.clear();
	col3.clear();
	col2.clear();
	allCols.clear();
	col1.clear();
	col4.clear();
	col5.clear();
	grounds.clear();
	groundGroup.clear();
	entityPos.clear();
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateBasicGeometry();
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//buffer size initialization
	unsigned int size = sizeof(VertexShaderExternalData);
	size = (size + 15) / 16 * 16;

	//buffer description
	D3D11_BUFFER_DESC cbDesc = {}; 
	cbDesc.BindFlags= D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.ByteWidth= size;
	cbDesc.CPUAccessFlags= D3D11_CPU_ACCESS_WRITE;
	cbDesc.Usage= D3D11_USAGE_DYNAMIC;


	angle = 0.0f;
	scaleSize = 1;

	//creating initial position and orientation of camera
	XMFLOAT3 pos = { 0,2.5f,-1 };
	XMFLOAT3 orient{ 0.5f,0,0 };

	cam = new Camera(pos, orient, (float)this->width/this->height);

	//creating the three directional lights and one point light
	light = DirectionalLight();
	light.ambientColor = XMFLOAT3(1.01f, 0.5f, 1.1f);
	light.diffuseColor = XMFLOAT3(1.0f, 1.1f, 1.1f);
	light.direction = XMFLOAT3(1, -1, 0);

	light2 = DirectionalLight();
	light2.ambientColor = XMFLOAT3(0.01f, 0.01f, 0.1f);
	light2.diffuseColor = XMFLOAT3(1.0f, 0.1f, 0.1f);
	light2.direction = XMFLOAT3(-1, 1, 0);

	light3 = DirectionalLight();
	light3.ambientColor = XMFLOAT3(0.01f, 0.01f, 0.01f);
	light3.diffuseColor = XMFLOAT3(1.0f, 1.0f, 0.1f);
	light3.direction = XMFLOAT3(0, 1, -1);

	point1 = PointLight();
	point1.ambientColor = XMFLOAT3(0.01f, 0.01f, 0.01f);
	point1.diffuseColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	point1.position = XMFLOAT3(0, 5, 0);

	doneInput = false;

	m_font = std::make_unique<SpriteFont>(device.Get(), L"myfile.spritefont");
	m_spriteBatch = std::make_unique<SpriteBatch>(context.Get());

	m_fontPos.x =  -0.0f;
	m_fontPos.y =  -0.0f;

	playerDead = false;

	score = 0.0f;

	speedMult = 1.0f;
	benchMark = 0;
	
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"VertexShader.cso").c_str());
	pixelShader = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	pixelShaderNormal = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"pixelShaderNormal.cso").c_str());
	vertexShaderNormal = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"vertexShaderNormal.cso").c_str());

}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	

	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself

	//first shape vertexs and indices
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.5f, +0.5f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f),XMFLOAT3(0,0,-1), XMFLOAT2(0,0)  },
		{ XMFLOAT3(0.3f, -0.5f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
	};

	// Set up the indices, which tell us which vertices to use and in which order
	// - This is somewhat redundant for just 3 vertices (it's a simple example)
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2 };

	//second shape vertexs and indices
	Vertex vertices2[] = {
		{ XMFLOAT3(-0.7f, +0.1f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(-0.4f, -0.9f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(-0.7f, -0.9f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(-0.4f, -0.2f, +0.3f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
	};

	unsigned int indices2[] = { 0, 1, 2, 1, 2, 3 };

	//third shape vertexs and indices
	Vertex vertices3[] = {
		{ XMFLOAT3(-0.2f, +0.8f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0)},
		{ XMFLOAT3(+0.1f, -0.0f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(-0.2f, -0.3f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(-0.0f, +0.5f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(-0.0f, +0.5f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
	};

	

	unsigned int indices3[] = { 0, 1, 2, 1, 2, 3, 0, 4, 2 };

	//get textures from files
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/grass.jpg").c_str(), 0, texture2SRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/rock.PNG").c_str(), 0, textureSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/rock_normals.png").c_str(), 0, normalSRV.GetAddressOf());
	
	//pbr textures
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/cobblestone_albedo.png").c_str(), 0, cobbleA.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/cobblestone_normals.png").c_str(), 0, cobbleN.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/cobblestone_roughness.png").c_str(), 0, cobbleR.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/cobblestone_metal.png").c_str(), 0, cobbleM.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/floor_albedo.png").c_str(), 0, floorA.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/floor_normals.png").c_str(), 0, floorN.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/floor_roughness.png").c_str(), 0, floorR.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/floor_metal.png").c_str(), 0, floorM.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/paint_albedo.png").c_str(), 0, paintA.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/paint_normals.png").c_str(), 0, paintN.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/paint_roughness.png").c_str(), 0, paintR.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/paint_metal.png").c_str(), 0, paintM.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/scratched_albedo.png").c_str(), 0, scratchedA.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/scratched_normals.png").c_str(), 0, scratchedN.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/scratched_roughness.png").c_str(), 0, scratchedR.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/scratched_metal.png").c_str(), 0, scratchedM.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/bronze_albedo.png").c_str(), 0, bronzeA.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/bronze_normals.png").c_str(), 0, bronzeN.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/bronze_roughness.png").c_str(), 0, bronzeR.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/bronze_metal.png").c_str(), 0, bronzeM.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/rough_albedo.png").c_str(), 0, roughA.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/rough_normals.png").c_str(), 0, roughN.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/rough_roughness.png").c_str(), 0, roughR.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/rough_metal.png").c_str(), 0, roughM.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/wood_albedo.png").c_str(), 0, woodA.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/wood_normals.png").c_str(), 0, woodN.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/wood_roughness.png").c_str(), 0, woodR.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../models/textures/pbr/wood_metal.png").c_str(), 0, woodM.GetAddressOf());
	
	//set sampler description
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//create sampler state
	device->CreateSamplerState(&samplerDesc, &sampler);
	
	

	//intializing materials, each with a different color tint
	mat2 = new Material(XMFLOAT4(1, 0, 0, 1), pixelShaderNormal, vertexShaderNormal, 100, woodA, sampler, true, woodN, woodM, woodR);
	mat4 = new Material(XMFLOAT4(1, 1, 0, 1), pixelShaderNormal, vertexShaderNormal, 20, bronzeA, sampler, true, bronzeN, bronzeM, bronzeR);
	mat3 = new Material(XMFLOAT4(1, 1, 1, 1), pixelShaderNormal, vertexShaderNormal, 512, paintA, sampler, true, paintN, paintM, paintR);
	mat1 = new Material(XMFLOAT4(0, 1, 1, 1), pixelShaderNormal, vertexShaderNormal, 30, roughA, sampler, true, roughN, roughM, roughR);
	mat5 = new Material(XMFLOAT4(1, 0, 1, 1), pixelShaderNormal, vertexShaderNormal, 400, floorA, sampler, true, floorN, floorM, floorR);
	mat6 = new Material(XMFLOAT4(1, 0, 1, 1), pixelShaderNormal, vertexShaderNormal, 400, cobbleA, sampler, true, cobbleN, cobbleM, cobbleR);
	mat7 = new Material(XMFLOAT4(1, 0, 1, 1), pixelShader, vertexShader, 100, texture2SRV, sampler, false, nullptr, nullptr, nullptr);

	//create mesh for sky
	Mesh* skyMesh = new Mesh(GetFullPathTo("../../models/cube.obj").c_str(), device);

	//initialize sky object, vertex, and pixel shaders
	skyObj = new Sky(skyMesh, sampler.Get(), device.Get());
	skyObj->simpleVertex = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"vertexShaderSky.cso").c_str());
	skyObj->simplePixel = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"pixelShaderSky.cso").c_str());
	
	//import texture for skybox
	CreateDDSTextureFromFile(device.Get(), GetFullPathTo_Wide(L"../../models/textures/SunnyCubeMap.dds").c_str(), nullptr, skyObj->shaderView.GetAddressOf());


	//initialize objects with models
	obj2 = new Mesh(GetFullPathTo("../../models/cube.obj").c_str(), device);
	obj3 = new Mesh(GetFullPathTo("../../models/cube.obj").c_str(), device);

	obj1 = new Mesh(GetFullPathTo("../../models/cube.obj").c_str(), device);
	obj4 = new Mesh(GetFullPathTo("../../models/cylinder.obj").c_str(), device);
	obj5 = new Mesh(GetFullPathTo("../../models/cube.obj").c_str(), device);
	obj6 = new Mesh(GetFullPathTo("../../models/cube.obj").c_str(), device);


	
	//game entity initialization
	g1 = new gameEntity(obj1, mat1, true);
	player = new gameEntity(obj4, mat4, true);
	ground = new gameEntity(obj1, mat7, true);


	//set the initial positions of the objects
	player->GetTransform()->SetPosition(0, 1.0f, 1);
	player->GetTransform()->SetScale(0.8f, 1.0f, 0.8f);


	//creating obstacles and putting them in arrays corresponding to each of the five lanes
	allCols.push_back(col1);
	allCols.push_back(col2);
	allCols.push_back(col3);
	allCols.push_back(col4);
	allCols.push_back(col5);
	int count = -1;

	for (auto& m : allCols) {
		for (int i = 0; i <10; i++) {
			m.push_back(new gameEntity(obj1, mat2, false));
			if (i > 0) {
				m[i]->GetTransform()->SetPosition(count, 1.0f, m[i - 1.0f]->GetTransform()->GetPosition().z + (rand() % 15)+7);

			}
			else {
				m[i]->GetTransform()->SetPosition(count, 1.0f, 10 + rand()%10);
			}
			if (m[i]->GetTransform()->GetPosition().z < 30) {
				m[i]->isActive = true;
			}
			m[i]->GetTransform()->SetScale(1, 1 * rand() % 2 + 1, 1);
			entityPos.push_back(m[i]);
		}
		count++;
	}

	//creating terrain (ground, platform, walls)
	for (int i = 0; i <= 5;i++) {
		std::vector<gameEntity*> temp;
		temp.push_back(new gameEntity(obj1, mat1, true));
		temp.push_back(new gameEntity(obj1, mat7, true));
		temp.push_back(new gameEntity(obj1, mat6, true));
		temp.push_back(new gameEntity(obj1, mat6, true));
		temp.push_back(new gameEntity(obj1, mat6, true));
		temp.push_back(new gameEntity(obj1, mat6, true));
		temp.push_back(new gameEntity(obj1, mat6, true));
		temp.push_back(new gameEntity(obj1, mat6, true));
		grounds.push_back(temp);
		grounds[i][0]->GetTransform()->SetScale(5, 0.5f, 30);
		grounds[i][0]->GetTransform()->SetPosition(1, 0.3f, 1+(30*i));

		grounds[i][1]->GetTransform()->SetScale(80,0.1f,30);
		grounds[i][1]->GetTransform()->SetPosition(1, 0.0f, 1 + (30 * i));

		grounds[i][2]->GetTransform()->SetPosition(-3.5f, 1, 1 + (30*i));
		grounds[i][3]->GetTransform()->SetPosition(-3.5f, 1, 1 + 15+(30*i));
		grounds[i][5]->GetTransform()->SetPosition(5.5f, 1, 1 + 15+(30*i));
		grounds[i][4]->GetTransform()->SetPosition(5.5f, 1, 1 + (30*i));
		grounds[i][2]->GetTransform()->SetScale(4, 1.75f, 15);
		grounds[i][3]->GetTransform()->SetScale(4, 1.75f, 15);
		grounds[i][4]->GetTransform()->SetScale(4, 1.75f, 15);
		grounds[i][5]->GetTransform()->SetScale(4, 1.75f, 15);

		
		
	}
	

	//g1->GetTransform()->SetScale(5, 0.1f, 50);
	//ground->GetTransform()->SetPosition(1, 0.0f, 1);
	//ground->GetTransform()->SetScale(50, 0.1f, 50);



	//filling the entities array

	entities.push_back(player);
	entities.push_back(ground);
	//entities.push_back(wall);
	//entities.push_back(wall2);

	ready = true;

}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	cam->UpdateProjectionMatrix((float)this->width/this->height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();


	//variables for transform
	angle += deltaTime;
	scaleSize += deltaTime;
	
	if (ready) {
		cam->Update(deltaTime, hWnd, player->GetTransform());

	}

	
	

	if (!playerDead) {

		//terrain movement code
		int tempCount = 0;
		for (int i = 0; i < grounds.size(); i++) {
			grounds[i][0]->GetTransform()->MoveAbsolute(0, 0, -2.5 * deltaTime * speedMult);
			grounds[i][1]->GetTransform()->MoveAbsolute(0, 0, -2.5 * deltaTime * speedMult);
			grounds[i][2]->GetTransform()->MoveAbsolute(0, 0, -2.5 * deltaTime * speedMult);
			grounds[i][3]->GetTransform()->MoveAbsolute(0, 0, -2.5 * deltaTime * speedMult);
			grounds[i][4]->GetTransform()->MoveAbsolute(0, 0, -2.5 * deltaTime * speedMult);
			grounds[i][5]->GetTransform()->MoveAbsolute(0, 0, -2.5 * deltaTime * speedMult);
			if (grounds[i][0]->GetTransform()->GetPosition().z < -30) {
				XMFLOAT3 p = grounds[i][0]->GetTransform()->GetPosition();
				grounds[i][0]->GetTransform()->SetPosition(p.x, p.y, grounds.back()[0]->GetTransform()->GetPosition().z + 30);
				p = grounds[i][1]->GetTransform()->GetPosition();
				grounds[i][1]->GetTransform()->SetPosition(p.x, p.y, grounds.back()[1]->GetTransform()->GetPosition().z + 30);
				p = grounds[i][2]->GetTransform()->GetPosition();
				grounds[i][2]->GetTransform()->SetPosition(p.x, p.y, grounds.back()[2]->GetTransform()->GetPosition().z + 30);
				p = grounds[i][3]->GetTransform()->GetPosition();
				

				grounds[i][3]->GetTransform()->SetPosition(p.x, p.y, grounds.back()[3]->GetTransform()->GetPosition().z + 30+15);
				
				p = grounds[i][4]->GetTransform()->GetPosition();
				
				grounds[i][4]->GetTransform()->SetPosition(p.x, p.y, grounds.back()[4]->GetTransform()->GetPosition().z + 30);
				p = grounds[i][5]->GetTransform()->GetPosition();
				grounds[i][5]->GetTransform()->SetPosition(p.x, p.y, grounds.back()[5]->GetTransform()->GetPosition().z + 30+15);

				grounds.push_back(grounds[i]);
				grounds[i][0]->isActive = false;
				grounds[i][1]->isActive = false;
				grounds[i][3]->isActive = false;
				grounds[i][2]->isActive = false;
				grounds[i][4]->isActive = false;
				grounds[i][5]->isActive = false;
				grounds.erase(grounds.begin() + tempCount);

			}
			tempCount++;
			if (grounds[i][0]->GetTransform()->GetPosition().z <= 60) {
				grounds[i][0]->isActive = true;
				grounds[i][1]->isActive = true;
				grounds[i][2]->isActive = true;
				grounds[i][3]->isActive = true;
				grounds[i][4]->isActive = true;
				grounds[i][5]->isActive = true;
			}
		}

		//obstancle movment code for all columns
		for (auto& c : allCols) {
			for (int j = 0; j < c.size(); j++) {
				if (!c[j]->stationary) {
					c[j]->GetTransform()->MoveAbsolute(0, 0, -2.5f * deltaTime * speedMult);
					if (c[j]->GetTransform()->GetPosition().z < 30) {
						c[j]->isActive = true;
					}
					if (c[j]->GetTransform()->GetPosition().z < -1.3) {
						XMFLOAT3 p = c[j]->GetTransform()->position;
						c[j]->GetTransform()->SetPosition(p.x, p.y, c[c.size() - 1]->GetTransform()->GetPosition().z + (rand() % 15)+7);
						for(auto& p : entityPos)
						c[j]->isActive = false;
						c.push_back(c[j]);
						c.erase(c.begin() + j);
						int index = 0;
						for (int i = 0; i < entityPos.size(); i++) {
							if (abs(entityPos[i]->GetTransform()->GetPosition().z - c[j]->GetTransform()->GetPosition().z) < 2) {
								//c[j]->GetTransform()->SetPosition(c[j]->GetTransform()->GetPosition().x, c[j]->GetTransform()->GetPosition().y, c[j]->GetTransform()->GetPosition().z + 1);
							}
							if (entityPos[i] == c[j]) {
								index = i;
							}
						}
						entityPos.push_back(entityPos[index]);
						entityPos.erase(entityPos.begin() + index);
						
					}
					if ((abs((float)c[j]->GetTransform()->position.z - (float)player->GetTransform()->position.z)) <= 0.8f && c[j]->GetTransform()->position.x == player->GetTransform()->position.x) {
						//c[j]->GetTransform()->MoveAbsolute(10, 0, 0);
						c[j]->isActive = false;
						playerDead = true;
						if (score > highScore) {
							highScore = score;
						}
					}
				}
			}


		}
		//score code
		score += 100 * deltaTime;
		benchMark += deltaTime;
		light.ambientColor.y -= 0.001f*deltaTime;
		
		if (benchMark > 5.0f) { speedMult *= 1.15f; benchMark = 0.0f; }
	}
	else {
		if (GetAsyncKeyState('R') & 0x8000) {
			//restarts the game
			entities.clear();
			col3.clear();
			delete obj1;
			delete obj2;
			delete obj3;
			delete obj4;
			delete obj5;
			delete obj6;

			delete g1;
			delete g2;
			delete g3;
			delete player;
			delete g5;
			delete g6;
			delete ground;

			delete mat1;
			delete mat2;
			delete mat3;
			delete mat4;
			delete mat5;
			delete mat6;
			delete mat7;

			delete cam;

			delete vertexShader;
			delete pixelShader;

			delete vertexShaderNormal;
			delete pixelShaderNormal;

			delete skyObj;

			m_font.reset();
			m_spriteBatch.reset();


			entities.clear();
			col3.clear();
			col2.clear();
			allCols.clear();
			col1.clear();
			col4.clear();
			col5.clear();
			grounds.clear();
			entityPos.clear();
			Init();
		}
	}
	

	


	
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };



	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);


	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	//context->VSSetShader(vertexShader.Get(), 0, 0);
	//context->PSSetShader(pixelShader.Get(), 0, 0);


	// Ensure the pipeline knows how to interpret the data (numbers)
	// from the vertex buffer.  
	// - If all of your 3D models use the exact same vertex layout,
	//    this could simply be done once in Init()
	// - However, this isn't always the case (but might be for this course)
	//context->IASetInputLayout(inputLayout.Get());

	//initialize stride and offset
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	//draw sky
	skyObj->Draw(context, cam);
	//loop through entities and call draw functions after mapping constant buffer and setting struct values
	for (auto& m : entities)
	{
		//setting the pixel shader lights
		pixelShader->SetData("light", &light, sizeof(DirectionalLight));
		pixelShader->SetData("light2", &light2, sizeof(DirectionalLight));
		pixelShader->SetData("light3", &light3, sizeof(DirectionalLight));
		pixelShader->SetData("point1", &point1, sizeof(PointLight));
		pixelShader->SetData("cameraPos", &cam->GetTransform()->GetPosition(), sizeof(XMFLOAT3));
		pixelShader->SetData("specExponent", &m->mat->specExponent, sizeof(float));

		pixelShaderNormal->SetData("light", &light, sizeof(DirectionalLight));
		pixelShaderNormal->SetData("light2", &light2, sizeof(DirectionalLight));
		pixelShaderNormal->SetData("light3", &light3, sizeof(DirectionalLight));
		pixelShaderNormal->SetData("point1", &point1, sizeof(PointLight));
		pixelShaderNormal->SetData("cameraPos", &cam->GetTransform()->GetPosition(), sizeof(XMFLOAT3));
		pixelShaderNormal->SetData("specExponent", &m->mat->specExponent, sizeof(float));

		pixelShaderNormal->CopyAllBufferData();

		pixelShader->CopyAllBufferData();
		m->draw(context, stride, offset, cam);

	}

	//rendering for the obstacles
	for (auto& c : allCols) {
		for (auto& m : c) {
			if (m->isActive) {
				pixelShader->SetData("light", &light, sizeof(DirectionalLight));
				pixelShader->SetData("light2", &light2, sizeof(DirectionalLight));
				pixelShader->SetData("light3", &light3, sizeof(DirectionalLight));
				pixelShader->SetData("point1", &point1, sizeof(PointLight));
				pixelShader->SetData("cameraPos", &cam->GetTransform()->GetPosition(), sizeof(XMFLOAT3));
				pixelShader->SetData("specExponent", &m->mat->specExponent, sizeof(float));

				pixelShaderNormal->SetData("light", &light, sizeof(DirectionalLight));
				pixelShaderNormal->SetData("light2", &light2, sizeof(DirectionalLight));
				pixelShaderNormal->SetData("light3", &light3, sizeof(DirectionalLight));
				pixelShaderNormal->SetData("point1", &point1, sizeof(PointLight));
				pixelShaderNormal->SetData("cameraPos", &cam->GetTransform()->GetPosition(), sizeof(XMFLOAT3));
				pixelShaderNormal->SetData("specExponent", &m->mat->specExponent, sizeof(float));

				pixelShaderNormal->CopyAllBufferData();

				pixelShader->CopyAllBufferData();
				m->draw(context, stride, offset, cam);
			}
			
		}
	}

	//rendering for the terrain
	for (auto& g : grounds) {
		for (auto& s : g) {
			if (s->isActive) {
				pixelShader->SetData("light", &light, sizeof(DirectionalLight));
				pixelShader->SetData("light2", &light2, sizeof(DirectionalLight));
				pixelShader->SetData("light3", &light3, sizeof(DirectionalLight));
				pixelShader->SetData("point1", &point1, sizeof(PointLight));
				pixelShader->SetData("cameraPos", &cam->GetTransform()->GetPosition(), sizeof(XMFLOAT3));
				pixelShader->SetData("specExponent", &s->mat->specExponent, sizeof(float));

				pixelShaderNormal->SetData("light", &light, sizeof(DirectionalLight));
				pixelShaderNormal->SetData("light2", &light2, sizeof(DirectionalLight));
				pixelShaderNormal->SetData("light3", &light3, sizeof(DirectionalLight));
				pixelShaderNormal->SetData("point1", &point1, sizeof(PointLight));
				pixelShaderNormal->SetData("cameraPos", &cam->GetTransform()->GetPosition(), sizeof(XMFLOAT3));
				pixelShaderNormal->SetData("specExponent", &s->mat->specExponent, sizeof(float));

				pixelShaderNormal->CopyAllBufferData();

				pixelShader->CopyAllBufferData();
				s->draw(context, stride, offset, cam);
			}
		}
		
	}
	
	//creating and rendering the on screen text
	m_spriteBatch->Begin();
	std::string str = std::to_string(score);
	char vOut[17];
	_gcvt_s(vOut, sizeof(vOut), score, 8);
	wchar_t v[17];
	mbstowcs_s(NULL, v, sizeof(v) / 2, vOut, sizeof(vOut));
	const wchar_t* output = v;

	char vOut2[17];
	_gcvt_s(vOut2, sizeof(vOut2), highScore, 8);
	wchar_t v2[17];
	mbstowcs_s(NULL, v2, sizeof(v2) / 2, vOut2, sizeof(vOut2));
	const wchar_t* highScoreOutput = v2;
	
	const wchar_t* gameOver = L"You Died\nPress R to retry";
	const wchar_t* scoreText = L"Score: ";
	const wchar_t* highScoreText = L"HighScore: ";
	

	DirectX::SimpleMath::Vector2 origin = m_font->MeasureString(output) / 2.f;

	m_font->DrawString(m_spriteBatch.get(), output,
		DirectX::SimpleMath::Vector2::Vector2(320,50), Colors::White, 0.f, origin);

	m_font->DrawString(m_spriteBatch.get(), scoreText,
		DirectX::SimpleMath::Vector2::Vector2(150, 50), Colors::White, 0.f, origin);
	
	m_font->DrawString(m_spriteBatch.get(), highScoreText,
		DirectX::SimpleMath::Vector2::Vector2(900, 50), Colors::LightYellow, 0.f, origin);

	m_font->DrawString(m_spriteBatch.get(), highScoreOutput,
		DirectX::SimpleMath::Vector2::Vector2(1180, 50), Colors::LightYellow, 0.f, origin);

	if (playerDead) {
		m_font->DrawString(m_spriteBatch.get(), gameOver,
			DirectX::SimpleMath::Vector2::Vector2(200, 500), Colors::Red, 0.f, origin);
	}

	m_spriteBatch->End();

	

	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	//  - for this demo, this step *could* simply be done once during Init(),
	//    but I'm doing it here because it's often done multiple times per frame
	//    in a larger application/game

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());

	
}