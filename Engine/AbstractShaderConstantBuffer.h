#pragma once

#include <d3d11_1.h>
#include <sstream>

#include "Memory.h"

class AbstractShaderConstantBuffer
{
public:
	virtual bool Init(__in ID3D11Device* pDevice) = 0;

	virtual ~AbstractShaderConstantBuffer(void)
	{
		using namespace std;

		wostringstream debugStream;
		debugStream << "~AbstractShaderConstantBuffer called" << endl;
		OutputDebugString(debugStream.str().c_str());
	}

	virtual void UpdateVSConstantBuffer(__in ID3D11DeviceContext* pDeviceContext)
	{
		pDeviceContext->VSSetConstantBuffers(bufferPosition, 1, buffer.getAddress());
	}

	virtual void UpdatePSConstantBuffer(__in ID3D11DeviceContext* pDeviceContext)
	{
		pDeviceContext->PSSetConstantBuffers(bufferPosition, 1, buffer.getAddress());
	}

	virtual bool UpdateConstantBuffer(__in ID3D11DeviceContext* pDeviceContext) = 0;

protected:
	AbstractShaderConstantBuffer(unsigned int position) : bufferPosition(position) {}

protected:
	unsigned int bufferPosition;
	com_ptr<ID3D11Buffer> buffer;
};