#pragma once

#include <DirectXMath.h>
#include <comdef.h>

#include "AbstractShaderConstantBuffer.h"

using namespace DirectX;

class MatricesConstantBuffer : AbstractShaderConstantBuffer
{
public:
	MatricesConstantBuffer(unsigned int position, __in CXMMATRIX worldMatrix, __in CXMMATRIX viewMatrix, __in CXMMATRIX projectionMatrix);
	MatricesConstantBuffer(unsigned int position);

	virtual ~MatricesConstantBuffer(void);

	void SetWorldMatrix(__in CXMMATRIX worldMatrix) { _bufferPointer->World = worldMatrix; }
	void SetViewMatrix(__in CXMMATRIX viewMatrix) { _bufferPointer->View = viewMatrix; }
	void SetProjectionMatrix(__in CXMMATRIX projectionMatrix) { _bufferPointer->Projection = projectionMatrix; }

	CXMMATRIX GetWorldMatrix(void) const { return _bufferPointer->World; }
	CXMMATRIX GetViewMatrix(void) const { return _bufferPointer->View; }
	CXMMATRIX GetProjectionMatrix(void) const { return _bufferPointer->Projection; }

	bool Init(__in ID3D11Device* pDevice) override;
	bool UpdateConstantBuffer(__in ID3D11DeviceContext* pDeviceContext) override;

private:
	typedef __declspec(align(16)) struct MATRIX_BUFFER
	{
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
	} MATRIX_BUFFER, *PMATRIX_BUFFER;

	PMATRIX_BUFFER _bufferPointer;
};