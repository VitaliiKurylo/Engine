#pragma once

#include <d3d11_1.h>
#include <DirectXColors.h>

#include <comdef.h>

#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <string>

#include "WindowController.h"
#include "..\Memory.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace DirectX;

template class ENGINE_API com_ptr<ID3D11Device>;
template class ENGINE_API com_ptr<ID3D11DeviceContext>;
template class ENGINE_API com_ptr<IDXGISwapChain>;
template class ENGINE_API com_ptr<ID3D11RenderTargetView>;

template class ENGINE_API com_ptr<ID3D11Texture2D>;
template class ENGINE_API com_ptr<ID3D11DepthStencilState>;
template class ENGINE_API com_ptr<ID3D11DepthStencilView>;
template class ENGINE_API com_ptr<ID3D11RasterizerState>;

class ENGINE_API D3D11Controller : public WindowController
{
public:
	D3D11Controller(__in HINSTANCE hInstance);
	virtual ~D3D11Controller(void);

	bool Init(void) override;

	bool OnResized(__in unsigned int width, __in unsigned int height) override;

protected:
	unsigned int videoCardMemory;
	bool vsyncEnabled;
	bool fullscreenEnabled;

	com_ptr<ID3D11Device> device;
	com_ptr<ID3D11DeviceContext> deviceContext;

private:
	com_ptr<IDXGISwapChain> _swapChain;
	com_ptr<ID3D11RenderTargetView> _renderTargetView;

	com_ptr<ID3D11Texture2D> _depthStencilBuffer;
	com_ptr<ID3D11DepthStencilState> _depthStencilState;
	com_ptr<ID3D11DepthStencilView> _depthStencilView;
	com_ptr<ID3D11RasterizerState> _rasterState;

	DXGI_SWAP_CHAIN_DESC* _swapChainDesc;
	D3D11_TEXTURE2D_DESC* _depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC* _depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC* _depthStencilViewDesc;
	D3D11_RASTERIZER_DESC* _rasterDesc;

	D3D11_VIEWPORT* _viewPort;

protected:
	void Clear(void);
	void Present(void);
};
