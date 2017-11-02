#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Frustrum
{
public:
	Frustrum();
	virtual ~Frustrum();

	bool Init(__in float screenDepth, __in CXMMATRIX projectionMatrix, __in CXMMATRIX viewMatrix);
};

