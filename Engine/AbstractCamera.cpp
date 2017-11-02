#include "AbstractCamera.h"

/// <summary>
/// Initializes a new instance of the <see cref="Camera"/> class.
/// </summary>
AbstractCamera::AbstractCamera(void) : startPosition({ 0.0f, 0.0f, 0.0f }),
position({ 1.0f, 0.0f, 0.0f }),
startFocusPoint({ 0.0f, 0.0f, 0.0f }),
focusPoint({ 0.0f, 0.0f, 0.0f }),
fieldOfView(XM_PI / 4.0f),
aspectRatio(1.0f),
nearPlane(1.0f),
farPlane(1000.0f),
mouseWheelDelta(0),
currentButtonMask(0),
dragRectangle({ LONG_MIN, LONG_MIN, LONG_MAX, LONG_MAX }),
mouseLeftButtonDown(false),
mouseMiddleButtonDown(false),
mouseRightButtonDown(false)
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();

	GetCursorPos(&mousePosition);
}

/// <summary>
/// Finalizes an instance of the <see cref="Camera"/> class.
/// </summary>
AbstractCamera::~AbstractCamera(void)
{
}

void AbstractCamera::UpdateViewMatrix(void)
{
	viewMatrix = XMMatrixLookAtLH(position, focusPoint, { 0.0f, 1.0f, 0.0f, 0.0f });

	auto inversedMatrix = XMMatrixInverse(nullptr, viewMatrix);

	XMVECTOR upVector = inversedMatrix.r[2];
	auto upVectorX = XMVectorGetX(upVector);
	auto upVectorY = XMVectorGetY(upVector);
	auto upVectorZ = XMVectorGetZ(upVector);

	cameraYawAngle = atan2f(upVectorX, upVectorZ);
	cameraPitchAngle = -atan2f(upVectorY, sqrtf(upVectorZ * upVectorZ + upVectorX * upVectorX));
}

void AbstractCamera::UpdateProjectionMatrix(void)
{
	projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearPlane, farPlane);
}

LRESULT AbstractCamera::MessageProcecure(__in HWND hWnd, __in unsigned int msg, __in WPARAM wParam, __in LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(lParam);

	switch (msg)
	{
	case WM_KEYDOWN:
	{
		break;
	}

	case WM_KEYUP:
	{
		break;
	}

	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	{
		POINT cursorPosition = { (short)LOWORD(lParam), (short)HIWORD(lParam) };

		if ((msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) && PtInRect(&dragRectangle, cursorPosition))
		{
			mouseLeftButtonDown = true; currentButtonMask |= MOUSE_LEFT_BUTTON;
		}
		if ((msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) && PtInRect(&dragRectangle, cursorPosition))
		{
			mouseMiddleButtonDown = true; currentButtonMask |= MOUSE_MIDDLE_BUTTON;
		}
		if ((msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) && PtInRect(&dragRectangle, cursorPosition))
		{
			mouseRightButtonDown = true; currentButtonMask |= MOUSE_RIGHT_BUTTON;
		}

		SetCapture(hWnd);
		GetCursorPos(&mousePosition);
		return 1;
	}

	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_LBUTTONUP:
	{
		if (msg == WM_LBUTTONUP)
		{
			mouseLeftButtonDown = false; currentButtonMask &= ~MOUSE_LEFT_BUTTON;
		}
		if (msg == WM_MBUTTONUP)
		{
			mouseMiddleButtonDown = false; currentButtonMask &= ~MOUSE_MIDDLE_BUTTON;
		}
		if (msg == WM_RBUTTONUP)
		{
			mouseRightButtonDown = false; currentButtonMask &= ~MOUSE_RIGHT_BUTTON;
		}
		if (!mouseLeftButtonDown && !mouseMiddleButtonDown && !mouseRightButtonDown)
		{
			ReleaseCapture();
		}
		break;
	}

	case WM_CAPTURECHANGED:
	{
		if ((HWND)lParam != hWnd)
		{
			if ((currentButtonMask & MOUSE_LEFT_BUTTON) || (currentButtonMask & MOUSE_MIDDLE_BUTTON) || (currentButtonMask & MOUSE_RIGHT_BUTTON))
			{
				mouseLeftButtonDown = false;
				mouseMiddleButtonDown = false;
				mouseRightButtonDown = false;
				currentButtonMask &= ~MOUSE_LEFT_BUTTON;
				currentButtonMask &= ~MOUSE_MIDDLE_BUTTON;
				currentButtonMask &= ~MOUSE_RIGHT_BUTTON;
				ReleaseCapture();
			}
		}
		break;
	}

	case WM_MOUSEWHEEL:
	{
		mouseWheelDelta += (short)HIWORD(wParam);
		break;
	}
	}

	return 0;
}
