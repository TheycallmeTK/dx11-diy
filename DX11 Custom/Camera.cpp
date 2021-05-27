#include "Camera.h"
Camera::Camera(DirectX::XMFLOAT3 initialPos, DirectX::XMFLOAT3 orientation, float aspectRatio)
{
	trans = Transform();
	trans.SetPosition(initialPos.x, initialPos.y, initialPos.z);
	trans.SetRotation(orientation.x, orientation.y, orientation.z);
	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
	moveSpeed = 0.2f;
	mouseLookSpeed = 2;
	inputDoing = false;
}

DirectX::XMFLOAT4X4 Camera::getView()
{
	return view;
}

DirectX::XMFLOAT4X4 Camera::getProj()
{
	return proj;
}

Transform* Camera::GetTransform()
{
	return &trans;
}

void Camera::moveSideways()
{

}

void Camera::UpdateViewMatrix()
{
	

	//getting new rotation and direction vectors for view matrix
	DirectX::XMFLOAT3 rot = trans.GetPitchYawRoll();
	DirectX::XMVECTOR dir = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0, 0, 1, 0), DirectX::XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rot)));

	DirectX::XMMATRIX viewM = DirectX::XMMatrixLookToLH(XMLoadFloat3(&trans.GetPosition()), dir, DirectX::XMVectorSet(0, 1, 0, 0));

	//setting updated view matrix
	DirectX::XMStoreFloat4x4(&view, viewM);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	DirectX::XMStoreFloat4x4(&proj, DirectX::XMMatrixPerspectiveFovLH(1.7f, aspectRatio, 0.1f, 500));
}



void Camera::Update(float dt, HWND windowHandle, Transform* t)
{
	int key = 0;


	
	//input buffer and getting key input
	if (GetKeyState('A') >= 0 && GetKeyState('D') >= 0) {
		inputDoing = false;
		key = 0;
	}

	if (inputDoing) 
	{ 
		return; 
	}

	if (GetKeyState('D') < 0) {
		key = 1;
		inputDoing = true;
	}

	if (GetKeyState('A') < 0) {
		key = 2;
		inputDoing = true;
	}

	//scale speed by deltatime
	float speed = this->moveSpeed * dt / 10;

	float timer;
	DirectX::XMFLOAT3 movement = DirectX::XMFLOAT3(0,0,0);
	DirectX::XMFLOAT3 playerMove = DirectX::XMFLOAT3(0, 0, 0);

	//lane switching for camera and player

	if (key == 1) 
	{ 
		movement = trans.GetPosition();
		printf("%6.4lf", movement.x);
		if (movement.x < 3.0f) {
			trans.SetPosition(movement.x + 1.0f, movement.y, movement.z);
			playerMove = t->GetPosition();
			t->SetPosition(playerMove.x + 1.0f, playerMove.y, playerMove.z);
		}

		
	}
	if (key == 2) 
	{ 
		movement = trans.GetPosition();
		printf("%6.4lf", movement.x);
		if (movement.x > -1.0f) {
			trans.SetPosition(movement.x - 1.0f, movement.y, movement.z);
			playerMove = t->GetPosition();
			t->SetPosition(playerMove.x - 1.0f, playerMove.y, playerMove.z);
		}
	}

	
	

	
	
	//key event functions
	//if(GetAsyncKeyState('D') & 0x8000){ trans.MoveRelative(speed, 0, 0); }
	//if (GetAsyncKeyState('A') & 0x8000) { trans.MoveRelative(-speed, 0, 0); }
	if(GetAsyncKeyState('W') & 0x8000) { trans.MoveRelative(0, 0, speed); }
	if(GetAsyncKeyState('S') & 0x8000) { trans.MoveRelative(0, 0, -speed); }
	
	if(GetAsyncKeyState(VK_SPACE) & 0x8000) { trans.MoveAbsolute(0, speed, 0); }
	if(GetAsyncKeyState('X') & 0x8000) { trans.MoveAbsolute(0, -speed, 0); }



	//getting the mouse position and moving view matrix if necessary
	POINT mousePos = {};
	GetCursorPos(&mousePos);
	ScreenToClient(windowHandle, &mousePos);


	if(GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		trans.Rotate((mousePos.y - prevMousePos.y) * mouseLookSpeed * dt, (mousePos.x - prevMousePos.x) * mouseLookSpeed * dt, 0);
		
	}

	prevMousePos = mousePos;

	UpdateViewMatrix();


}


