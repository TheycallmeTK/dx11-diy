#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <wrl/event.h>
#include "DXCore.h"
#include <DirectXMath.h>
#include "Transform.h"



class Camera
{
	public:
		Camera(DirectX::XMFLOAT3 intialPos, DirectX::XMFLOAT3 orientation, float aspectRatio);
		void Update(float dt, HWND windowHandle, Transform* t);
		void UpdateProjectionMatrix(float aspectRatio);
		void UpdateViewMatrix();
		DirectX::XMFLOAT4X4 getView();
		DirectX::XMFLOAT4X4 getProj();
		Transform* GetTransform();
		void moveSideways();
		bool inputDoing;

	private:
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 proj;
		Transform trans;
		float moveSpeed;
		float mouseLookSpeed;
		POINT prevMousePos;
		float fov;
		
		

		
		
};

