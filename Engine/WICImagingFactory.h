#pragma once

#include <comdef.h>
#include <Wincodec.h>

#include "Memory.h"

class WICImagingFactory
{
public:
	static IWICImagingFactory* Instance();
	static unsigned int BitsPerPixel(REFGUID targetGuid);
	static bool IsWICImagingFactory2() { return isWICImagingFactory2; }

private:
	WICImagingFactory(void) {}

	WICImagingFactory(WICImagingFactory const& value) = delete;
	void operator=(WICImagingFactory const& value) = delete;

private:
	static bool isWICImagingFactory2;
};