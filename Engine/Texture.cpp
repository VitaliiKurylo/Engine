#include "Texture.h"

Texture::Texture(__in_z const wchar_t* filename) : textureFilename(filename)
{
}

Texture::~Texture(void)
{
}

bool Texture::Init(__in ID3D11Device* device)
{
	return CreateTextureFromFile(device, nullptr, textureFilename);
}

bool Texture::CreateTextureFromFile(__in ID3D11Device* device, __in_opt ID3D11DeviceContext* deviceContext, __in_z const wchar_t* filename, __in size_t maxsize)
{
	assert(device);

	auto imagingFactory = WICImagingFactory::Instance();
	if (imagingFactory == nullptr)
	{
		return false;
	}

	com_ptr<IWICBitmapDecoder> decoder;
	auto hresult = imagingFactory->CreateDecoderFromFilename(filename, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, decoder.getAddress());
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	com_ptr<IWICBitmapFrameDecode> frame;
	hresult = decoder->GetFrame(0, frame.getAddress());
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	if (CreateTextureFromWICBitmapFrame(device, nullptr, frame.get(), maxsize, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, false))
	{
#ifdef _DEBUG
		wstring bufferName(filename);
		if (_resourceView)
		{
			_resourceView->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<unsigned int>(bufferName.size()), bufferName.c_str());
		}
#endif

		return true;
	}

	return false;
}

bool Texture::CreateTextureFromWICBitmapFrame(__in ID3D11Device* device, __in_opt ID3D11DeviceContext* deviceContext,
	__in IWICBitmapFrameDecode* frame,
	__in size_t maxsize, __in D3D11_USAGE usage, __in D3D11_BIND_FLAG bindFlags, __in unsigned int cpuAccessFlags, __in unsigned int miscFlags, __in bool forceSRGB)
{
	unsigned int width, height;

	auto hresult = frame->GetSize(&width, &height);
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	assert(width > 0 && height > 0);

	if (maxsize <= 0)
	{
		switch (device->GetFeatureLevel())
		{
		case D3D_FEATURE_LEVEL_9_1:
		case D3D_FEATURE_LEVEL_9_2:
			maxsize = 2048 /*D3D_FL9_1_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
			break;

		case D3D_FEATURE_LEVEL_9_3:
			maxsize = 4096 /*D3D_FL9_3_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
			break;

		case D3D_FEATURE_LEVEL_10_0:
		case D3D_FEATURE_LEVEL_10_1:
			maxsize = 8192 /*D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
			break;

		default:
			maxsize = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
			break;
		}
	}

	assert(maxsize > 0);

	if (width > maxsize || height > maxsize)
	{
		auto aspectRatio = static_cast<float>(height) / static_cast<float>(width);
		if (width > height)
		{
			textureWidth = static_cast<unsigned int>(maxsize);
			textureHeight = static_cast<unsigned int>(static_cast<float>(maxsize) * aspectRatio);
		}
		else
		{
			textureHeight = static_cast<unsigned int>(maxsize);
			textureWidth = static_cast<unsigned int>(static_cast<float>(maxsize) / aspectRatio);
		}
		assert(textureWidth <= maxsize && textureHeight <= maxsize);
	}
	else
	{
		textureWidth = width;
		textureHeight = height;
	}

	// Determine format
	WICPixelFormatGUID pixelFormat = {};
	hresult = frame->GetPixelFormat(&pixelFormat);
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	WICPixelFormatGUID convertGUID = {};
	memcpy(&convertGUID, &pixelFormat, sizeof(WICPixelFormatGUID));

	unsigned int bitsPerPixel = 0;

	auto format = WIC2DXGI(pixelFormat);
	if (format == DXGI_FORMAT_UNKNOWN)
	{
		if (memcmp(&GUID_WICPixelFormat96bppRGBFixedPoint, &pixelFormat, sizeof(WICPixelFormatGUID)) == 0)
		{
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
			if (WICImagingFactory::IsWICImagingFactory2())
			{
				memcpy(&convertGUID, &GUID_WICPixelFormat96bppRGBFloat, sizeof(WICPixelFormatGUID));
				format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else
#endif
			{
				memcpy(&convertGUID, &GUID_WICPixelFormat128bppRGBAFloat, sizeof(WICPixelFormatGUID));
				format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
		}
		else
		{
			for (size_t i = 0; i < _countof(WIC_PAIR_LIST); ++i)
			{
				if (memcmp(&WIC_PAIR_LIST[i].Source, &pixelFormat, sizeof(WICPixelFormatGUID)) == 0)
				{
					memcpy(&convertGUID, &WIC_PAIR_LIST[i].Target, sizeof(WICPixelFormatGUID));

					format = WIC2DXGI(WIC_PAIR_LIST[i].Target);
					assert(format != DXGI_FORMAT_UNKNOWN);
					bitsPerPixel = WICImagingFactory::BitsPerPixel(convertGUID);
					break;
				}
			}
		}

		if (format == DXGI_FORMAT_UNKNOWN)
		{
			_com_error error(HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));
			OutputDebugString(error.ErrorMessage());
			return false;
		}
	}
	else
	{
		bitsPerPixel = WICImagingFactory::BitsPerPixel(pixelFormat);
	}

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
	if (format == DXGI_FORMAT_R32G32B32_FLOAT && deviceContext)
	{
		// Special case test for optional device support for autogen mipchains for R32G32B32_FLOAT 
		unsigned int formatSupport = 0;
		hresult = device->CheckFormatSupport(DXGI_FORMAT_R32G32B32_FLOAT, &formatSupport);
		if (FAILED(hresult) || (formatSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN) == 0)
		{
			// Use R32G32B32A32_FLOAT instead which is required for Feature Level 10.0 and up
			memcpy(&convertGUID, &GUID_WICPixelFormat128bppRGBAFloat, sizeof(WICPixelFormatGUID));
			format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			bitsPerPixel = 128;
		}
	}
#endif

	if (bitsPerPixel == 0)
		return false;

	// Handle sRGB formats
	if (forceSRGB)
	{
		format = MakeSRGB(format);
	}
	else
	{
		com_ptr<IWICMetadataQueryReader> metadataQueryReader;
		if (SUCCEEDED(frame->GetMetadataQueryReader(metadataQueryReader.getAddress())))
		{
			GUID containerFormat = {};
			if (SUCCEEDED(metadataQueryReader->GetContainerFormat(&containerFormat)))
			{
				// Check for sRGB colorspace metadata
				bool sRGB = false;

				PROPVARIANT value = {};
				PropVariantInit(&value);

				if (memcmp(&containerFormat, &GUID_ContainerFormatPng, sizeof(GUID)) == 0)
				{
					// Check for sRGB chunk
					if (SUCCEEDED(metadataQueryReader->GetMetadataByName(L"/sRGB/RenderingIntent", &value)) && value.vt == VT_UI1)
					{
						sRGB = true;
					}
				}
				else if (SUCCEEDED(metadataQueryReader->GetMetadataByName(L"System.Image.ColorSpace", &value)) && value.vt == VT_UI2 && value.uiVal == 1)
				{
					sRGB = true;
				}

				PropVariantClear(&value);

				if (sRGB)
				{
					format = MakeSRGB(format);
				}
			}
		}
	}

	// Verify our target format is supported by the current device (handles WDDM 1.0 or WDDM 1.1 device driver cases as well as DirectX 11.0 Runtime without 16bpp format support)
	unsigned int support = 0;
	hresult = device->CheckFormatSupport(format, &support);
	if (FAILED(hresult) || !(support & D3D11_FORMAT_SUPPORT_TEXTURE2D))
	{
		// Fallback to RGBA 32-bit format which is supported by all devices
		memcpy(&convertGUID, &GUID_WICPixelFormat32bppRGBA, sizeof(WICPixelFormatGUID));
		format = DXGI_FORMAT_R8G8B8A8_UNORM;
		bitsPerPixel = 32;
	}

	// Allocate temporary memory for image
	auto rowPitch = textureWidth * bitsPerPixel;

	vector<unsigned char> rawImageData(rowPitch * textureHeight);

	// Load image data
	if (memcmp(&convertGUID, &pixelFormat, sizeof(GUID)) == 0 && textureWidth == width && textureHeight == height)
	{
		// No format conversion or resize needed
		hresult = frame->CopyPixels(0, rowPitch, static_cast<unsigned int>(rawImageData.size()), rawImageData.data());
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}
	}
	else if (textureWidth != width || textureHeight != height)
	{
		// Resize
		if (WICImagingFactory::Instance() == nullptr)
		{
			return false;
		}

		com_ptr<IWICBitmapScaler> bitmapScaler;
		hresult = WICImagingFactory::Instance()->CreateBitmapScaler(bitmapScaler.getAddress());
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		hresult = bitmapScaler->Initialize(frame, textureWidth, textureHeight, WICBitmapInterpolationModeFant);
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		WICPixelFormatGUID scalerPixelFormat;
		hresult = bitmapScaler->GetPixelFormat(&scalerPixelFormat);
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		if (memcmp(&convertGUID, &scalerPixelFormat, sizeof(GUID)) == 0)
		{
			// No format conversion needed
			hresult = bitmapScaler->CopyPixels(0, rowPitch, static_cast<unsigned int>(rawImageData.size()), rawImageData.data());
			if (FAILED(hresult))
			{
				_com_error error(hresult);
				OutputDebugString(error.ErrorMessage());
				return false;
			}
		}
		else
		{
			com_ptr<IWICFormatConverter> formatConverter;
			hresult = WICImagingFactory::Instance()->CreateFormatConverter(formatConverter.getAddress());
			if (FAILED(hresult))
			{
				_com_error error(hresult);
				OutputDebugString(error.ErrorMessage());
				return false;
			}

			auto canConvert = FALSE;
			hresult = formatConverter->CanConvert(scalerPixelFormat, convertGUID, &canConvert);
			if (FAILED(hresult) || canConvert == FALSE)
			{
				_com_error error(E_UNEXPECTED);
				OutputDebugString(error.ErrorMessage());
				return false;
			}

			hresult = formatConverter->Initialize(bitmapScaler.get(), convertGUID, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
			if (FAILED(hresult))
			{
				_com_error error(hresult);
				OutputDebugString(error.ErrorMessage());
				return false;
			}

			hresult = formatConverter->CopyPixels(0, rowPitch, static_cast<unsigned int>(rawImageData.size()), rawImageData.data());
			if (FAILED(hresult))
			{
				_com_error error(hresult);
				OutputDebugString(error.ErrorMessage());
				return false;
			}
		}
	}
	else
	{
		// Format conversion but no resize
		if (WICImagingFactory::Instance() == nullptr)
		{
			return false;
		}

		com_ptr<IWICFormatConverter> formatConverter;
		hresult = WICImagingFactory::Instance()->CreateFormatConverter(formatConverter.getAddress());
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		auto canConvert = FALSE;
		hresult = formatConverter->CanConvert(pixelFormat, convertGUID, &canConvert);
		if (FAILED(hresult) || canConvert == FALSE)
		{
			_com_error error(E_UNEXPECTED);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		hresult = formatConverter->Initialize(frame, convertGUID, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		hresult = formatConverter->CopyPixels(0, rowPitch, static_cast<unsigned int>(rawImageData.size()), rawImageData.data());
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}
	}

	// See if format is supported for auto-gen mipmaps (varies by feature level)
	auto generateMips = false;
	/*if (device)
	{
		unsigned int formatSupport = 0;
		hresult = device->CheckFormatSupport(format, &formatSupport);
		if (SUCCEEDED(hresult) && (formatSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
		{
			generateMips = true;
		}
	}*/

	// Create texture
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = (generateMips) ? 0 : 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = usage;
	textureDesc.CPUAccessFlags = cpuAccessFlags;

	if (generateMips)
	{
		textureDesc.BindFlags = bindFlags | D3D11_BIND_RENDER_TARGET;
		textureDesc.MiscFlags = miscFlags | D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}
	else
	{
		textureDesc.BindFlags = bindFlags;
		textureDesc.MiscFlags = miscFlags;
	}

	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = reinterpret_cast<void*>(rawImageData.data());
	subresourceData.SysMemPitch = rowPitch;
	subresourceData.SysMemSlicePitch = 0;

	com_ptr<ID3D11Texture2D> texture;
	hresult = device->CreateTexture2D(&textureDesc, generateMips ? nullptr : &subresourceData, texture.getAddress());
	if (SUCCEEDED(hresult))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels = generateMips ? -1 : 1;

		hresult = device->CreateShaderResourceView(texture.get(), &shaderResourceViewDesc, _resourceView.getAddress());
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		if (generateMips && deviceContext)
		{
			deviceContext->UpdateSubresource(texture.get(), 0, nullptr, rawImageData.data(), static_cast<unsigned int>(rowPitch), static_cast<unsigned int>(rawImageData.size()));
			deviceContext->GenerateMips(_resourceView.get());
		}

		return true;
	}
	else
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}
}
