#pragma once

#include <D3Dcompiler.h>

#include <fstream>
#include <sstream>
#include <memory>
#include <comdef.h>

#include "MatricesConstantBuffer.h"
#include "LightConstantBuffer.h"

using namespace std;

class Shader
{
public:
	Shader(__in MatricesConstantBuffer* const matricesBuffer, __in LightConstantBuffer* const lightBuffer);
	virtual ~Shader(void);

	bool Init(__in ID3D11Device* device);
	bool __fastcall Render(__in ID3D11DeviceContext* deviceContext, __in int indexCount, __in_opt ID3D11ShaderResourceView* texture);

private:
	bool _CompileShader(__in ID3D11Device* device, __in_z const wchar_t* vsFilename, __in_z const wchar_t* psFilename, __out ID3D10Blob** vertexShaderBuffer, __out ID3D10Blob** pixelShaderBuffer);
	bool _LoadPrecompiledShader(__in ID3D11Device* device, __in_z const wchar_t* vsFilename, __in_z const wchar_t* psFilename, __out ID3D10Blob** vertexShaderBuffer, __out ID3D10Blob** pixelShaderBuffer);
	void _OutputShaderErrorMessage(__in ID3D10Blob* errorMessage, __in_z const wchar_t* shaderFilename);

private:
	// Shader template
	template<class TShader> long _CreateShader(__in ID3D11Device* device, __in ID3DBlob* shaderBlob, __in ID3D11ClassLinkage* classLinkage);

	// Vertex shader template
	template<> long _CreateShader<ID3D11VertexShader>(__in ID3D11Device* device, __in ID3DBlob* shaderBlob, __in ID3D11ClassLinkage* classLinkage)
	{
		assert(device);
		assert(shaderBlob);

		auto hresult = device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), classLinkage, _vertexShader.getAddress());

#ifdef _DEBUG
		if (SUCCEEDED(hresult))
		{

		}
#endif

		return hresult;
	};

	// Pixel shader template
	template<> long _CreateShader<ID3D11PixelShader>(__in ID3D11Device* device, __in ID3DBlob* shaderBlob, __in ID3D11ClassLinkage* classLinkage)
	{
		assert(device);
		assert(shaderBlob);

		auto hresult = device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), classLinkage, _pixelShader.getAddress());

#ifdef _DEBUG
		if (SUCCEEDED(hresult))
		{

		}
#endif

		return hresult;
	};

private:
	MatricesConstantBuffer* const matricesConstantBuffer;
	LightConstantBuffer* const lightConstantBuffer;

	com_ptr<ID3D11VertexShader> _vertexShader;
	com_ptr<ID3D11PixelShader> _pixelShader;
	com_ptr<ID3D11InputLayout> _layout;
	com_ptr<ID3D11SamplerState> _samplerState;
};

