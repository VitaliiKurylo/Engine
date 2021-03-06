#pragma once

#include <d3d11_1.h>
#include <string>
#include <cassert>
#include <vector>

#include <comdef.h>

#include "WICImagingFactory.h"

using namespace std;

struct WIC_GUID_PAIR
{
	GUID Source;
	GUID Target;
};

static WIC_GUID_PAIR WIC_PAIR_LIST[] =
{
	// Note target GUID in this conversion table must be one of those directly supported formats (above).
	{ GUID_WICPixelFormatBlackWhite, GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM
	{ GUID_WICPixelFormat1bppIndexed, GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
	{ GUID_WICPixelFormat2bppIndexed, GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
	{ GUID_WICPixelFormat4bppIndexed, GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
	{ GUID_WICPixelFormat8bppIndexed, GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
	{ GUID_WICPixelFormat2bppGray, GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 
	{ GUID_WICPixelFormat4bppGray, GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 
	{ GUID_WICPixelFormat16bppGrayFixedPoint, GUID_WICPixelFormat16bppGrayHalf }, // DXGI_FORMAT_R16_FLOAT 
	{ GUID_WICPixelFormat32bppGrayFixedPoint, GUID_WICPixelFormat32bppGrayFloat }, // DXGI_FORMAT_R32_FLOAT 
	{ GUID_WICPixelFormat16bppBGR555, GUID_WICPixelFormat16bppBGRA5551 }, // DXGI_FORMAT_B5G5R5A1_UNORM
	{ GUID_WICPixelFormat32bppBGR101010, GUID_WICPixelFormat32bppRGBA1010102 }, // DXGI_FORMAT_R10G10B10A2_UNORM
	{ GUID_WICPixelFormat24bppBGR, GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
	{ GUID_WICPixelFormat24bppRGB, GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
	{ GUID_WICPixelFormat32bppPBGRA, GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
	{ GUID_WICPixelFormat32bppPRGBA, GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
	{ GUID_WICPixelFormat48bppRGB, GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat48bppBGR, GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat64bppBGRA, GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat64bppPRGBA, GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat64bppPBGRA, GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat48bppRGBFixedPoint, GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
	{ GUID_WICPixelFormat48bppBGRFixedPoint, GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
	{ GUID_WICPixelFormat64bppRGBAFixedPoint, GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
	{ GUID_WICPixelFormat64bppBGRAFixedPoint, GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
	{ GUID_WICPixelFormat64bppRGBFixedPoint, GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
	{ GUID_WICPixelFormat64bppRGBHalf, GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
	{ GUID_WICPixelFormat48bppRGBHalf, GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
	{ GUID_WICPixelFormat128bppPRGBAFloat, GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
	{ GUID_WICPixelFormat128bppRGBFloat, GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
	{ GUID_WICPixelFormat128bppRGBAFixedPoint, GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
	{ GUID_WICPixelFormat128bppRGBFixedPoint, GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
	{ GUID_WICPixelFormat32bppRGBE, GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
	{ GUID_WICPixelFormat32bppCMYK, GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
	{ GUID_WICPixelFormat64bppCMYK, GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat40bppCMYKAlpha, GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat80bppCMYKAlpha, GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

#if(_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
	{ GUID_WICPixelFormat32bppRGB, GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat64bppRGB, GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat64bppPRGBAHalf, GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
#endif
};

struct WIC_FORMAT_PAIR
{
	GUID Source;
	DXGI_FORMAT Format;
};

static WIC_FORMAT_PAIR WIC_FORMAT_PAIR_LIST[] =
{
	{ GUID_WICPixelFormat128bppRGBAFloat, DXGI_FORMAT_R32G32B32A32_FLOAT },
	{ GUID_WICPixelFormat64bppRGBAHalf, DXGI_FORMAT_R16G16B16A16_FLOAT },
	{ GUID_WICPixelFormat64bppRGBA, DXGI_FORMAT_R16G16B16A16_UNORM },
	{ GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM },
	{ GUID_WICPixelFormat32bppBGRA, DXGI_FORMAT_B8G8R8A8_UNORM }, // DXGI 1.1
	{ GUID_WICPixelFormat32bppBGR, DXGI_FORMAT_B8G8R8X8_UNORM }, // DXGI 1.1
	{ GUID_WICPixelFormat32bppRGBA1010102XR, DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM }, // DXGI 1.1
	{ GUID_WICPixelFormat32bppRGBA1010102, DXGI_FORMAT_R10G10B10A2_UNORM },
	{ GUID_WICPixelFormat16bppBGRA5551, DXGI_FORMAT_B5G5R5A1_UNORM },
	{ GUID_WICPixelFormat16bppBGR565, DXGI_FORMAT_B5G6R5_UNORM },
	{ GUID_WICPixelFormat32bppGrayFloat, DXGI_FORMAT_R32_FLOAT },
	{ GUID_WICPixelFormat16bppGrayHalf, DXGI_FORMAT_R16_FLOAT },
	{ GUID_WICPixelFormat16bppGray, DXGI_FORMAT_R16_UNORM },
	{ GUID_WICPixelFormat8bppGray, DXGI_FORMAT_R8_UNORM },
	{ GUID_WICPixelFormat8bppAlpha, DXGI_FORMAT_A8_UNORM },
};


static DXGI_FORMAT WIC2DXGI(const GUID& guid)
{
	for (size_t i = 0; i < _countof(WIC_FORMAT_PAIR_LIST); ++i)
	{
		if (memcmp(&WIC_FORMAT_PAIR_LIST[i].Source, &guid, sizeof(GUID)) == 0)
			return WIC_FORMAT_PAIR_LIST[i].Format;
	}

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
	if (WICImagingFactory::IsWICImagingFactory2())
	{
		if (memcmp(&GUID_WICPixelFormat96bppRGBFloat, &guid, sizeof(GUID)) == 0)
			return DXGI_FORMAT_R32G32B32_FLOAT;
	}
#endif
	return DXGI_FORMAT_UNKNOWN;
}

static DXGI_FORMAT MakeSRGB(__in DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	case DXGI_FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM_SRGB;

	case DXGI_FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM_SRGB;

	case DXGI_FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM_SRGB;

	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

	case DXGI_FORMAT_B8G8R8X8_UNORM:
		return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

	case DXGI_FORMAT_BC7_UNORM:
		return DXGI_FORMAT_BC7_UNORM_SRGB;

	default:
		return format;
	}
}

template class com_ptr<ID3D11ShaderResourceView>;

class Texture
{
public:
	Texture(__in_z const wchar_t* filename);
	virtual ~Texture(void);

	bool Init(__in ID3D11Device* device);

	ID3D11ShaderResourceView* GetTexture(void) const { return _resourceView.get(); };

private:
	com_ptr<ID3D11ShaderResourceView> _resourceView;

protected:
	const wchar_t* textureFilename;
	unsigned int textureHeight;
	unsigned int textureWidth;

protected:
	bool __cdecl CreateTextureFromFile(__in ID3D11Device* device, __in_opt ID3D11DeviceContext* deviceContext, __in_z const wchar_t* filename, __in size_t maxsize = 0);

	bool CreateTextureFromWICBitmapFrame(__in ID3D11Device* device, __in_opt ID3D11DeviceContext* deviceContext,
		__in IWICBitmapFrameDecode* frame,
		__in size_t maxsize, __in D3D11_USAGE usage, __in D3D11_BIND_FLAG bindFlags, __in unsigned int cpuAccessFlags, __in unsigned int miscFlags, __in bool forceSRGB);
};

