#include "D3D11Controller.h"

/// <summary>
/// Initializes a new instance of the <see cref="D3D11Controller"/> class.
/// </summary>
/// <param name="hInstance">The h instance.</param>
D3D11Controller::D3D11Controller(__in HINSTANCE hInstance) : WindowController(hInstance),
videoCardMemory(0),
vsyncEnabled(false),
fullscreenEnabled(false),
_swapChainDesc(new DXGI_SWAP_CHAIN_DESC()),
_depthBufferDesc(new D3D11_TEXTURE2D_DESC()),
_depthStencilDesc(new D3D11_DEPTH_STENCIL_DESC()),
_depthStencilViewDesc(new D3D11_DEPTH_STENCIL_VIEW_DESC()),
_rasterDesc(new D3D11_RASTERIZER_DESC()),
_viewPort(new D3D11_VIEWPORT())
{
}

/// <summary>
/// Finalizes an instance of the <see cref="D3D11Controller"/> class.
/// </summary>
D3D11Controller::~D3D11Controller(void)
{
#ifdef _DEBUG
	if (device)
	{
		com_ptr<ID3D11Debug> debug;
		device->QueryInterface(debug.getAddress());
		debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
#endif
	if (deviceContext)
	{
		deviceContext->ClearState();
	}

	if (_swapChain)
	{
		// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
		_swapChain->SetFullscreenState(false, nullptr);
	}

	if (_swapChainDesc)
	{
		delete _swapChainDesc;
		_swapChainDesc = nullptr;
	}

	if (_depthBufferDesc)
	{
		delete _depthBufferDesc;
		_depthBufferDesc = nullptr;
	}

	if (_depthStencilDesc)
	{
		delete _depthStencilDesc;
		_depthStencilDesc = nullptr;
	}

	if (_depthStencilViewDesc)
	{
		delete _depthStencilViewDesc;
		_depthStencilViewDesc = nullptr;
	}

	if (_rasterDesc)
	{
		delete _rasterDesc;
		_rasterDesc = nullptr;
	}

	if (_viewPort)
	{
		delete _viewPort;
		_viewPort = nullptr;
	}

	wostringstream debugStream;
	debugStream << "~D3D11Controller called" << endl;
	OutputDebugString(debugStream.str().c_str());
}

/// <summary>
/// Initializes this instance.
/// </summary>
/// <returns></returns>
bool D3D11Controller::Init(void)
{
	using namespace std;

	if (!WindowController::Init())
	{
		return false;
	}

	assert(appWindow);

	unsigned int numerator = 0, denominator = 1;

	{
		com_ptr<IDXGIFactory> factory;
		// Create a DirectX graphics interface factory.
		auto hresult = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.getAddress()));
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		com_ptr<IDXGIAdapter> adapter;
		// Use the factory to create an adapter for the primary graphics interface (video card).
		hresult = factory->EnumAdapters(0, adapter.getAddress());
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		com_ptr<IDXGIOutput> adapterOutput;
		// Enumerate the primary adapter output (monitor).
		hresult = adapter->EnumOutputs(0, adapterOutput.getAddress());
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		unsigned int numModes = 0;
		// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
		hresult = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		// Create a list to hold all the possible display modes for this monitor/video card combination.
		vector<DXGI_MODE_DESC> displayModes(numModes);
		hresult = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModes.data());
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		// Now go through all the display modes and find the one that matches the screen width and height.
		// When a match is found store the numerator and denominator of the refresh rate for that monitor.
		auto displayModeIterator = find_if(displayModes.begin(), displayModes.end(), [this](const DXGI_MODE_DESC& element)
		{
			return element.Width == clientWidth && element.Height == clientHeight;
		});
		if (displayModeIterator == displayModes.end())
		{
			OutputDebugString(L"Suitable display mode was not found\n");
			return false;
		}

		DXGI_ADAPTER_DESC adapterDesc = {};

		// Get the adapter (video card) description.
		hresult = adapter->GetDesc(&adapterDesc);
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		// Store the dedicated video card memory in megabytes.
		videoCardMemory = static_cast<unsigned int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		{
			wostringstream videoCardDescriptionStream;
			videoCardDescriptionStream << "Video card: " << adapterDesc.Description << endl;
			videoCardDescriptionStream << "Video memory: " << to_wstring(videoCardMemory) << "MB" << endl;
			OutputDebugString(videoCardDescriptionStream.str().c_str());
		}
	}

	RECT clientRect = {};
	GetClientRect(appWindow, &clientRect);

	// Compute the exact client dimensions. This will be used to initialize the render targets for our swap chain.
	clientWidth = clientRect.right - clientRect.left;
	clientHeight = clientRect.bottom - clientRect.top;

	// Initialize the swap chain description.
	ZeroMemory(_swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	_swapChainDesc->BufferCount = 2;
	_swapChainDesc->BufferDesc.Width = clientWidth;
	_swapChainDesc->BufferDesc.Height = clientHeight;

	// Set regular 32-bit surface for the back buffer.
	_swapChainDesc->BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if (vsyncEnabled)
	{
		_swapChainDesc->BufferDesc.RefreshRate.Numerator = numerator;
		_swapChainDesc->BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		_swapChainDesc->BufferDesc.RefreshRate.Numerator = 0;
		_swapChainDesc->BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	_swapChainDesc->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	_swapChainDesc->OutputWindow = appWindow;

	// Turn multisampling off.
	_swapChainDesc->SampleDesc.Count = 1;
	_swapChainDesc->SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if (fullscreenEnabled)
	{
		_swapChainDesc->Windowed = false;
	}
	else
	{
		_swapChainDesc->Windowed = true;
	}

	_swapChainDesc->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	unsigned int createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1
	};

	// Create the swap chain, Direct3D device, and Direct3D device context.
	auto hresult = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
		featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
		_swapChainDesc, _swapChain.resetAndGetAddress(), device.resetAndGetAddress(), nullptr, deviceContext.resetAndGetAddress());
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	// Get the pointer to the back buffer.
	com_ptr<ID3D11Texture2D> backBufferTexture;
	hresult = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBufferTexture.getAddress()));
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	// Create the render target view with the back buffer pointer.
	hresult = device->CreateRenderTargetView(backBufferTexture.get(), nullptr, _renderTargetView.getAddress());
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	// Initialize the description of the depth buffer.
	ZeroMemory(_depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	// Set up the description of the depth buffer.
	_depthBufferDesc->Width = clientWidth;
	_depthBufferDesc->Height = clientHeight;
	_depthBufferDesc->MipLevels = 1;
	_depthBufferDesc->ArraySize = 1;
	_depthBufferDesc->Format = DXGI_FORMAT_D32_FLOAT;
	_depthBufferDesc->SampleDesc.Count = 1;
	_depthBufferDesc->SampleDesc.Quality = 0;
	_depthBufferDesc->Usage = D3D11_USAGE_DEFAULT;
	_depthBufferDesc->BindFlags = D3D11_BIND_DEPTH_STENCIL;
	_depthBufferDesc->CPUAccessFlags = 0;
	_depthBufferDesc->MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	hresult = device->CreateTexture2D(_depthBufferDesc, nullptr, _depthStencilBuffer.resetAndGetAddress());
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	// Initialize the description of the stencil state.
	ZeroMemory(_depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	_depthStencilDesc->DepthEnable = true;
	_depthStencilDesc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	_depthStencilDesc->DepthFunc = D3D11_COMPARISON_LESS;

	_depthStencilDesc->StencilEnable = true;
	_depthStencilDesc->StencilReadMask = 0xFF;
	_depthStencilDesc->StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	_depthStencilDesc->FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	_depthStencilDesc->FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	_depthStencilDesc->FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	_depthStencilDesc->FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	_depthStencilDesc->BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	_depthStencilDesc->BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	_depthStencilDesc->BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	_depthStencilDesc->BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	hresult = device->CreateDepthStencilState(_depthStencilDesc, _depthStencilState.resetAndGetAddress());
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	// Set the depth stencil state.
	deviceContext->OMSetDepthStencilState(_depthStencilState.get(), 1);

	// Initialize the depth stencil view.
	ZeroMemory(_depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	// Set up the depth stencil view description.
	_depthStencilViewDesc->Format = DXGI_FORMAT_D32_FLOAT;
	_depthStencilViewDesc->ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	_depthStencilViewDesc->Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	hresult = device->CreateDepthStencilView(_depthStencilBuffer.get(), _depthStencilViewDesc, _depthStencilView.resetAndGetAddress());
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, _renderTargetView.getAddress(), _depthStencilView.get());

	ZeroMemory(_rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

	// Setup the raster description which will determine how and what polygons will be drawn.
	_rasterDesc->AntialiasedLineEnable = false;
	_rasterDesc->CullMode = D3D11_CULL_BACK;
	_rasterDesc->DepthBias = 0;
	_rasterDesc->DepthBiasClamp = 0.0f;
	_rasterDesc->DepthClipEnable = true;
	_rasterDesc->FillMode = D3D11_FILL_SOLID;
	_rasterDesc->FrontCounterClockwise = false;
	_rasterDesc->MultisampleEnable = false;
	_rasterDesc->ScissorEnable = false;
	_rasterDesc->SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	hresult = device->CreateRasterizerState(_rasterDesc, _rasterState.resetAndGetAddress());
	if (FAILED(hresult))
	{
		_com_error error(hresult);
		OutputDebugString(error.ErrorMessage());
		return false;
	}

	// Now set the rasterizer state.
	deviceContext->RSSetState(_rasterState.get());

	ZeroMemory(_viewPort, sizeof(D3D11_VIEWPORT));

	// Setup the viewport for rendering.
	_viewPort->Width = static_cast<float>(clientWidth);
	_viewPort->Height = static_cast<float>(clientHeight);
	_viewPort->MinDepth = 0.0f;
	_viewPort->MaxDepth = 1.0f;
	_viewPort->TopLeftX = 0.0f;
	_viewPort->TopLeftY = 0.0f;

	// Create the viewport.
	deviceContext->RSSetViewports(1, _viewPort);

	return true;
}

/// <summary>
/// Called when [resized].
/// </summary>
/// <param name="width">The width.</param>
/// <param name="height">The height.</param>
/// <returns></returns>
bool D3D11Controller::OnResized(__in unsigned int width, __in unsigned int height)
{
	if (_swapChain)
	{
		deviceContext->OMSetRenderTargets(0, nullptr, nullptr);

		_renderTargetView.reset();

		_depthStencilView.reset();

		auto hresult = _swapChain->ResizeBuffers(0, clientWidth = width, clientHeight = height, DXGI_FORMAT_UNKNOWN, 0);
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		{
			// Get the pointer to the back buffer.
			com_ptr<ID3D11Texture2D> backBufferTexture;
			hresult = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBufferTexture.getAddress()));
			if (FAILED(hresult))
			{
				_com_error error(hresult);
				OutputDebugString(error.ErrorMessage());
				return false;
			}

			// Create the render target view with the back buffer pointer.
			hresult = device->CreateRenderTargetView(backBufferTexture.get(), nullptr, _renderTargetView.getAddress());
			if (FAILED(hresult))
			{
				_com_error error(hresult);
				OutputDebugString(error.ErrorMessage());
				return false;
			}
		}

		// Set up the description of the depth buffer.
		_depthBufferDesc->Width = clientWidth;
		_depthBufferDesc->Height = clientHeight;

		// Create the texture for the depth buffer using the filled out description.
		hresult = device->CreateTexture2D(_depthBufferDesc, nullptr, _depthStencilBuffer.resetAndGetAddress());
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		// Create the depth stencil view.
		hresult = device->CreateDepthStencilView(_depthStencilBuffer.get(), _depthStencilViewDesc, _depthStencilView.getAddress());
		if (FAILED(hresult))
		{
			_com_error error(hresult);
			OutputDebugString(error.ErrorMessage());
			return false;
		}

		deviceContext->OMSetRenderTargets(1, _renderTargetView.getAddress(), _depthStencilView.get());

		// Setup the viewport for rendering.
		_viewPort->Width = static_cast<float>(clientWidth);
		_viewPort->Height = static_cast<float>(clientHeight);

		// Create the viewport.
		deviceContext->RSSetViewports(1, _viewPort);
	}

	return true;
}

/// <summary>
/// Begins this instance.
/// </summary>
void D3D11Controller::Clear(void)
{
	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(_renderTargetView.get(), Colors::CadetBlue);

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(_depthStencilView.get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

/// <summary>
/// Ends this instance.
/// </summary>
void D3D11Controller::Present(void)
{
	// Present the back buffer to the screen since rendering is complete.
	_swapChain->Present(vsyncEnabled ? 1 : 0, 0);
}