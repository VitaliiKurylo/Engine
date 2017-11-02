#pragma once

#include "D3D11Controller.h"
#include "..\AbstractCamera.h"
#include "..\TexturedTerrain.h"
#include "..\Texture.h"
#include "..\Shader.h"

#include <DirectXMath.h>
#include <comdef.h>

using namespace DirectX;

class ENGINE_API D3D11RenderController : public D3D11Controller
{
public:
	D3D11RenderController(__in HINSTANCE hInstance);
	virtual ~D3D11RenderController(void);

	bool Init(void) override;
	void Render(void) override;

	LRESULT MessageProcecure(__in HWND hWnd, __in unsigned int msg, __in WPARAM wParam, __in LPARAM lParam) override;

	bool OnResized(__in unsigned int width, __in unsigned int height) override;

protected:
	XMMATRIX worldMatrix;
	XMMATRIX orthoMatrix;

	AbstractCamera* camera;
	IRenderable* renderable;

	MatricesConstantBuffer* matricesConstantBuffer;
	LightConstantBuffer* lightConstantBuffer;
	Texture* texture;
	Shader* shader;
};

