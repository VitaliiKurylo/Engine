#pragma once

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <comdef.h>

#include "AbstractShaderConstantBuffer.h"

using namespace DirectX;

typedef __declspec(align(16)) struct MATERIAL
{
	float Ambient;
	float Diffuse;
	float Specular;
	float Shininess;
} MATERIAL, *PMATERIAL;

typedef __declspec(align(16)) struct DIRECTIONAL_LIGHT
{
	XMFLOAT4 Color;
	XMFLOAT3 Direction;
} DIRECTIONAL_LIGHT, *PDIRECTIONAL_LIGHT;

class LightConstantBuffer : AbstractShaderConstantBuffer
{
public:
	LightConstantBuffer(unsigned int position);
	virtual ~LightConstantBuffer(void);

	bool Init(__in ID3D11Device* pDevice) override;
	bool UpdateConstantBuffer(__in ID3D11DeviceContext* pDeviceContext) override;

	XMFLOAT4 GetAmbientLightColor(void) const { return _bufferPointer->AmbientLightColor; };
	MATERIAL GetMaterial(void) const { return _bufferPointer->Material; };
	DIRECTIONAL_LIGHT GetLight(void) const { return _bufferPointer->Light; };
	XMVECTOR GetCameraPosition(void) const { return XMLoadFloat4(&_bufferPointer->CameraPosition); };

	void SetAmbientLightColor(__in const XMFLOAT4& ambient) { _bufferPointer->AmbientLightColor = ambient; };
	void SetMaterial(__in const MATERIAL& material) { _bufferPointer->Material = material; };
	void SetLight(__in const DIRECTIONAL_LIGHT& light) { _bufferPointer->Light = light; };
	void SetCameraPosition(__in CXMVECTOR position) { XMStoreFloat4(&_bufferPointer->CameraPosition, position); };

private:
	typedef __declspec(align(16)) struct LIGHT_BUFFER
	{
		DIRECTIONAL_LIGHT Light;
		MATERIAL Material;
		XMFLOAT4 AmbientLightColor;
		XMFLOAT4 CameraPosition;
	} LIGHT_BUFFER, *PLIGHT_BUFFER;

	PLIGHT_BUFFER _bufferPointer;
};

