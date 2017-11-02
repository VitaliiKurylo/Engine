#pragma once

#include <d3d11_1.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <comdef.h>

#include <vector>
#include <cassert>
#include <sstream>

#include "Memory.h"
#include "IRenderable.h"

using namespace DirectX;

typedef struct VERTEX_DESC
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT4 Color;
	XMFLOAT2 Texture;
} VERTEX_DESC, *PVERTEX_DESC;

class AbstractModel : public IRenderable
{
public:
	virtual ~AbstractModel(void) {};

	virtual bool Init(__in ID3D11Device* device) = 0;
	virtual void Render(__in ID3D11DeviceContext* deviceContext);

	unsigned int GetIndexCount(void) const { return _indexCount; }

protected:
	com_ptr<ID3D11Buffer> _vertexBuffer;
	com_ptr<ID3D11Buffer> _indexBuffer;
	unsigned int _vertexCount;
	unsigned int _indexCount;

protected:
	AbstractModel(void) {};
	bool InitBuffers(__in ID3D11Device* device, const PVERTEX_DESC vertices, const unsigned long* indices);
};

