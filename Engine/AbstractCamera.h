#pragma once

#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

#define MOUSE_LEFT_BUTTON 0x01
#define MOUSE_MIDDLE_BUTTON 0x02
#define MOUSE_RIGHT_BUTTON 0x04
#define MOUSE_WHEEL 0x08

class AbstractCamera
{
public:
	AbstractCamera(void);
	virtual ~AbstractCamera(void);

	virtual LRESULT MessageProcecure(__in HWND hWnd, __in unsigned int msg, __in WPARAM wParam, __in LPARAM lParam);

	void SetPosition(__in CXMVECTOR externalPosition)
	{
		position = externalPosition;
		startPosition = externalPosition;
		UpdateViewMatrix();
	}

	void SetFocusPoint(__in CXMVECTOR externalFocusPoint)
	{
		focusPoint = externalFocusPoint;
		startFocusPoint = externalFocusPoint;
		UpdateViewMatrix();
	}

	CXMVECTOR GetPosition(void) const { return position; }
	CXMVECTOR GetFocusPoint(void) const { return focusPoint; }

	float GetFieldOfView(void) const { return fieldOfView; }
	float GetAspectRatio(void) const { return aspectRatio; }
	float GetNearPlane(void) const { return nearPlane; }
	float GetFarPlane(void) const { return farPlane; }

	void SetFieldOfView(float externalFieldOfView) { fieldOfView = externalFieldOfView; UpdateProjectionMatrix(); }
	void SetAspectRatio(float externalAspectRatio) { aspectRatio = externalAspectRatio; UpdateProjectionMatrix(); }
	void SetNearPlane(float externalNearPlane) { nearPlane = externalNearPlane; UpdateProjectionMatrix(); }
	void SetFarPlane(float externalFarPlane) { farPlane = externalFarPlane; UpdateProjectionMatrix(); }

	CXMMATRIX GetViewMatrix(void) const { return viewMatrix; }
	CXMMATRIX GetProjectionMatrix(void) const { return projectionMatrix; }

protected:
	void UpdateViewMatrix(void);
	void UpdateProjectionMatrix(void);

protected:
	XMVECTOR startPosition;
	XMVECTOR position;
	XMVECTOR startFocusPoint;
	XMVECTOR focusPoint;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	float fieldOfView;
	float aspectRatio;
	float nearPlane;
	float farPlane;

	float cameraYawAngle;
	float cameraPitchAngle;

	int mouseWheelDelta;
	int currentButtonMask;
	POINT mousePosition;
	RECT dragRectangle;
	bool mouseLeftButtonDown;
	bool mouseMiddleButtonDown;
	bool mouseRightButtonDown;
};

