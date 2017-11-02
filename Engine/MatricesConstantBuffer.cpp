#include "MatricesConstantBuffer.h"

MatricesConstantBuffer::MatricesConstantBuffer(unsigned int position, __in CXMMATRIX worldMatrix, __in CXMMATRIX viewMatrix, __in CXMMATRIX projectionMatrix) : AbstractShaderConstantBuffer(position),
_bufferPointer(new MATRIX_BUFFER())
{
	SetWorldMatrix(worldMatrix);
	SetViewMatrix(viewMatrix);
	SetProjectionMatrix(projectionMatrix);
}

MatricesConstantBuffer::MatricesConstantBuffer(unsigned int position) : MatricesConstantBuffer(position, XMMatrixIdentity(), XMMatrixIdentity(), XMMatrixIdentity())
{
}

MatricesConstantBuffer::~MatricesConstantBuffer(void)
{
	using namespace std;

	if (_bufferPointer)
	{
		delete _bufferPointer;
		_bufferPointer = nullptr;
	}

	wostringstream debugStream;
	debugStream << "~MatricesConstantBuffer called" << endl;
	OutputDebugString(debugStream.str().c_str());
}

bool MatricesConstantBuffer::Init(__in ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC bufferDescription = { sizeof(MATRIX_BUFFER), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };

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

bool MatricesConstantBuffer::UpdateConstantBuffer(__in ID3D11DeviceContext* pDeviceContext)
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

	// Get a pointer to the data in the constant buffer.
	auto dataPointer = reinterpret_cast<PMATRIX_BUFFER>(mappedResource.pData);

	// Copy the matrices into the constant buffer.
	dataPointer->World = XMMatrixTranspose(_bufferPointer->World);
	dataPointer->View = XMMatrixTranspose(_bufferPointer->View);
	dataPointer->Projection = XMMatrixTranspose(_bufferPointer->Projection);

	// Unlock the constant buffer.
	pDeviceContext->Unmap(buffer.get(), 0);

	return true;
}