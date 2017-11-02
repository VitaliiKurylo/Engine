#include "AbstractModel.h"

void AbstractModel::Render(__in ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	unsigned int stride = sizeof(VERTEX_DESC);
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.getAddress(), &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(_indexBuffer.get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

/// <summary>
/// Initializes the buffers.
/// </summary>
/// <param name="">The .</param>
/// <returns></returns>
bool AbstractModel::InitBuffers(__in ID3D11Device* device, const PVERTEX_DESC vertices, const unsigned long* indices)
{
	using namespace std;

	assert(device);
	assert(vertices);
	assert(indices);

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VERTEX_DESC) * _vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;

	// Now create the vertex buffer.
	auto hresult = device->CreateBuffer(&vertexBufferDesc, &vertexData, _vertexBuffer.resetAndGetAddress());
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	D3D11_BUFFER_DESC indexBufferDesc = {};
	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexData = {};
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;

	// Create the index buffer.
	hresult = device->CreateBuffer(&indexBufferDesc, &indexData, _indexBuffer.resetAndGetAddress());
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	return true;
}