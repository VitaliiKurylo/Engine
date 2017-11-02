#include "Shader.h"

/// <summary>
/// Initializes a new instance of the <see cref="Shader"/> class.
/// </summary>
Shader::Shader(__in MatricesConstantBuffer* const matricesBuffer, __in LightConstantBuffer* const lightBuffer) : matricesConstantBuffer(matricesBuffer),
lightConstantBuffer(lightBuffer)
{
}

/// <summary>
/// Finalizes an instance of the <see cref="Shader"/> class.
/// </summary>
Shader::~Shader(void)
{


}

/// <summary>
/// Initializes the specified device.
/// </summary>
/// <param name="device">The device.</param>
/// <param name="hwnd">The HWND.</param>
/// <returns></returns>
bool Shader::Init(__in ID3D11Device* device)
{
	using namespace std;

	assert(device);

	com_ptr<ID3D10Blob> vertexShaderBuffer;
	com_ptr<ID3D10Blob> pixelShaderBuffer;

	// For debug purposes get current working directory
	{
		wchar_t currentDirectory[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, currentDirectory);

		wostringstream shaderDebugStream;
		shaderDebugStream << "Current working directory: " << currentDirectory << endl;
		OutputDebugString(shaderDebugStream.str().c_str());
	}

	// Initialize the vertex and pixel shaders.

#ifdef _DEBUG
	if (!_CompileShader(device, L"../Engine/ColorVertexShader.hlsl", L"../Engine/ColorPixelShader.hlsl", vertexShaderBuffer.getAddress(), pixelShaderBuffer.getAddress()))
#else
	if (!_LoadPrecompiledShader(device, L"ColorVertexShader.cso", L"ColorPixelShader.cso", vertexShaderBuffer.getAddress(), pixelShaderBuffer.getAddress()))
#endif
	{
		return false;
	}

	// Create the vertex shader from the buffer.
	auto hresult = _CreateShader<ID3D11VertexShader>(device, vertexShaderBuffer.get(), nullptr);
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	// Create the pixel shader from the buffer.
	hresult = _CreateShader<ID3D11PixelShader>(device, pixelShaderBuffer.get(), nullptr);
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VERTEX_DESC stucture and in the shader.
	D3D11_INPUT_ELEMENT_DESC LAYOUT[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the vertex input layout.
	hresult = device->CreateInputLayout(LAYOUT, _countof(LAYOUT), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), _layout.resetAndGetAddress());
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MaxAnisotropy = 16;
	// Create the sampler state.
	hresult = device->CreateSamplerState(&samplerDesc, _samplerState.getAddress());
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	return true;
}

bool Shader::Render(__in ID3D11DeviceContext* deviceContext, __in int indexCount, __in_opt ID3D11ShaderResourceView* texture)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	// Vertex shader matrix buffer
	if (matricesConstantBuffer->UpdateConstantBuffer(deviceContext))
	{
		reinterpret_cast<AbstractShaderConstantBuffer*>(matricesConstantBuffer)->UpdateVSConstantBuffer(deviceContext);

		// Pixel shader light buffer
		if (lightConstantBuffer->UpdateConstantBuffer(deviceContext))
		{
			reinterpret_cast<AbstractShaderConstantBuffer*>(lightConstantBuffer)->UpdatePSConstantBuffer(deviceContext);

			ID3D11SamplerState* samplerStates[] = { _samplerState.get() };
			deviceContext->PSSetSamplers(0, 1, samplerStates);

			// Set shader texture resource in the pixel shader.
			ID3D11ShaderResourceView* textures[] = { texture };
			deviceContext->PSSetShaderResources(0, 1, textures);

			// Now render the prepared buffers with the shader.
			// Set the vertex input layout.
			deviceContext->IASetInputLayout(_layout.get());

			// Set the vertex and pixel shaders that will be used to render this triangle.
			deviceContext->VSSetShader(_vertexShader.get(), nullptr, 0);
			deviceContext->PSSetShader(_pixelShader.get(), nullptr, 0);

			// Render the triangle.
			deviceContext->DrawIndexed(indexCount, 0, 0);

			return true;
		}
	}

	return false;
}

bool Shader::_CompileShader(__in ID3D11Device* device, __in const wchar_t* vsFilename, __in const wchar_t* psFilename, __out ID3D10Blob** vertexShaderBuffer, __out ID3D10Blob** pixelShaderBuffer)
{
	com_ptr<ID3D10Blob> errorMessage;

	unsigned int flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	// Compile the vertex shader code.
	auto hresult = D3DCompileFromFile(vsFilename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", flags, 0, vertexShaderBuffer, errorMessage.getAddress());
	if (FAILED(hresult))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			_OutputShaderErrorMessage(errorMessage.get(), vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
		}

		return false;
	}

	// Compile the pixel shader code.
	hresult = D3DCompileFromFile(psFilename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", flags, 0, pixelShaderBuffer, errorMessage.resetAndGetAddress());
	if (FAILED(hresult))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			_OutputShaderErrorMessage(errorMessage.get(), psFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
		}

		return false;
	}

	return true;
}

bool Shader::_LoadPrecompiledShader(__in ID3D11Device* device, __in const wchar_t* vsFilename, __in const wchar_t* psFilename, __out ID3D10Blob** vertexShaderBuffer, __out ID3D10Blob** pixelShaderBuffer)
{
	assert(device);

	// Load the vertex shader code.
	auto hresult = D3DReadFileToBlob(vsFilename, vertexShaderBuffer);
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());

		return false;
	}

	// Load the pixel shader code.
	hresult = D3DReadFileToBlob(psFilename, pixelShaderBuffer);
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());

		return false;
	}

	return true;
}

/// <summary>
/// Outputs the shader error message.
/// </summary>
/// <param name="errorMessage">The error message.</param>
/// <param name="shaderFilename">The shader filename.</param>
void Shader::_OutputShaderErrorMessage(__in ID3D10Blob* errorMessage, __in const wchar_t* shaderFilename)
{
	// Open a file to write the error message to.
	ofstream fout("ShaderError.log");

	// Get a pointer to the error message text buffer.
	auto compileErrors = reinterpret_cast<LPCWSTR>(errorMessage->GetBufferPointer());

	OutputDebugString(compileErrors);

	fout << compileErrors;
	fout.close();
}