#pragma once

#include <d3d11_1.h>

class IRenderable
{
public:
	virtual bool Init(__in ID3D11Device* device) = 0;
	virtual void Render(__in ID3D11DeviceContext* deviceContext) = 0;
	virtual unsigned int GetIndexCount(void) const = 0;
};