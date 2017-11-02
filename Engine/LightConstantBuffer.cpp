#include "LightConstantBuffer.h"

LightConstantBuffer::LightConstantBuffer(unsigned int position) : AbstractShaderConstantBuffer(position),
_bufferPointer(new LIGHT_BUFFER())
{
}

LightConstantBuffer::~LightConstantBuffer(void)
{
	using namespace std;

	if (_bufferPointer)
	{
		delete _bufferPointer;
		_bufferPointer = nullptr;
	}

	wostringstream debugStream;
	debugStream << "~LightConstantBuffer called" << endl;
	OutputDebugString(debugStream.str().c_str());
}

bool LightConstantBuffer::Init(__in ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC bufferDescription = { sizeof(LIGHT_BUFFER), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };

	// Create the constant buffer pointer so we can access the shader constant buffer from within this class.
	auto hresult = pDevice->CreateBuffer(&bufferDescription, nullptr, buffer.resetAndGetAddress());
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	return true;
}

bool LightConstantBuffer::UpdateConstantBuffer(__in ID3D11DeviceContext* pDeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	// Lock the constant buffer so it can be written to.
	auto hresult = pDeviceContext->Map(buffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	XMStoreFloat3(&_bufferPointer->Light.Direction, XMVector3Normalize(XMLoadFloat3(&_bufferPointer->Light.Direction)));

	// Copy the values into the constant buffer.
	memcpy(reinterpret_cast<PLIGHT_BUFFER>(mappedResource.pData), _bufferPointer, sizeof(LIGHT_BUFFER));

	// Unlock the constant buffer.
	pDeviceContext->Unmap(buffer.get(), 0);

	return true;
}