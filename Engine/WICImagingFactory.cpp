#include "WICImagingFactory.h"

IWICImagingFactory* WICImagingFactory::Instance()
{
	static com_ptr<IWICImagingFactory> instance;

#if(_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
	auto hresult = CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory2), reinterpret_cast<void**>(instance.getAddress()));
	if (SUCCEEDED(hresult))
	{
		isWICImagingFactory2 = true;
	}
	else
	{
		hresult = CoCreateInstance(CLSID_WICImagingFactory1, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), reinterpret_cast<void**>(instance.getAddress()));
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
		}
	}
#else
	auto hresult = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), reinterpret_cast<void**>(instance.getAddress()));
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
	}
#endif

	return instance.get();
}

unsigned int WICImagingFactory::BitsPerPixel(REFGUID targetGuid)
{
	unsigned int bitsPerPixel = 0;
	auto factoryInstance = Instance();
	if (factoryInstance == nullptr)
	{
		return bitsPerPixel;
	}

	HRESULT hresult;
	com_ptr<IWICComponentInfo> componentInfo;
	if (FAILED(hresult = factoryInstance->CreateComponentInfo(targetGuid, componentInfo.getAddress())))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return bitsPerPixel;
	}

	WICComponentType componentType;
	if (FAILED(hresult = componentInfo->GetComponentType(&componentType)))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return bitsPerPixel;
	}

	if (componentType != WICPixelFormat)
	{
		return bitsPerPixel;
	}

	com_ptr<IWICPixelFormatInfo> pixelFormatInfo;
	if (FAILED(hresult = componentInfo->QueryInterface(pixelFormatInfo.getAddress())))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return bitsPerPixel;
	}

	if (FAILED(hresult = pixelFormatInfo->GetBitsPerPixel(&bitsPerPixel)))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return bitsPerPixel;
	}

	return bitsPerPixel;
}

bool WICImagingFactory::isWICImagingFactory2 = false;