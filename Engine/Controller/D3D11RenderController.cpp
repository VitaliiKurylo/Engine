#include "D3D11RenderController.h"

D3D11RenderController::D3D11RenderController(__in HINSTANCE hInstance) : D3D11Controller(hInstance),
camera(nullptr),
renderable(nullptr),
matricesConstantBuffer(new MatricesConstantBuffer(0)),
lightConstantBuffer(new LightConstantBuffer(0)),
texture(nullptr),
shader(nullptr)
{
}

/// <summary>
/// Finalizes an instance of the <see cref="D3D11RenderController"/> class.
/// </summary>
D3D11RenderController::~D3D11RenderController(void)
{
	if (camera)
	{
		delete camera;
		camera = nullptr;
	}

	if (renderable)
	{
		delete renderable;
		renderable = nullptr;
	}

	if (matricesConstantBuffer)
	{
		delete matricesConstantBuffer;
		matricesConstantBuffer = nullptr;
	}

	if (lightConstantBuffer)
	{
		delete lightConstantBuffer;
		lightConstantBuffer = nullptr;
	}

	if (texture)
	{
		delete texture;
		texture = nullptr;
	}

	if (shader)
	{
		delete shader;
		shader = nullptr;
	}

	wostringstream debugStream;
	debugStream << "~D3D11RenderController called" << endl;
	OutputDebugString(debugStream.str().c_str());
}

/// <summary>
/// Initializes this instance.
/// </summary>
/// <returns></returns>
bool D3D11RenderController::Init(void)
{
	if (!D3D11Controller::Init())
	{
		return false;
	}

	// Check for DirectX Math library support.
	if (!XMVerifyCPUSupport())
	{
		OutputDebugString(L"Failed to verify DirectX Math library support.\n");
		return false;
	}

	// Initialize the world matrix to the identity matrix.
	worldMatrix = XMMatrixIdentity();

	// Create an orthographic projection matrix for 2D rendering.
	orthoMatrix = XMMatrixOrthographicLH(static_cast<float>(clientWidth), static_cast<float>(clientHeight), 0.0f, 1000.0f);

	camera = new AbstractCamera();
	// Set the initial position of the camera.
	camera->SetPosition({ 0.0f, 300.0f, -300.0f });
	camera->SetFocusPoint({ 150.0f, 0.0f, 150.0f });
	camera->SetAspectRatio(static_cast<float>(clientWidth) / static_cast<float>(clientHeight));

	texture = new Texture(L"dirt2.jpg");
	if (!texture->Init(device.get()))
	{
		return false;
	}

	renderable = new TexturedTerrain(L"heightmap04.bmp");
	if (!renderable->Init(device.get()))
	{
		return false;
	}

	if (!matricesConstantBuffer->Init(device.get()))
	{
		return false;
	}

	matricesConstantBuffer->SetWorldMatrix(worldMatrix);
	matricesConstantBuffer->SetViewMatrix(camera->GetViewMatrix());
	matricesConstantBuffer->SetProjectionMatrix(camera->GetProjectionMatrix());

	if (!lightConstantBuffer->Init(device.get()))
	{
		return false;
	}

	lightConstantBuffer->SetCameraPosition(camera->GetPosition());
	lightConstantBuffer->SetMaterial({ 1.0f, 0.5f, 0.5f, 20.0f });
	lightConstantBuffer->SetLight({ XMFLOAT4(Colors::White), { 1.0f, -1.0f, 0.0f } });
	lightConstantBuffer->SetAmbientLightColor(XMFLOAT4(Colors::GhostWhite));

	shader = new Shader(matricesConstantBuffer, lightConstantBuffer);
	if (!shader->Init(device.get()))
	{
		OutputDebugString(L"Shader initialization failed");
		return false;
	}

	return true;
}

/// <summary>
/// Renders the specified delta time.
/// </summary>
/// <param name="deltaTime">The delta time.</param>
void D3D11RenderController::Render(void)
{
	D3D11Controller::Clear();

	renderable->Render(deviceContext.get());

	matricesConstantBuffer->SetViewMatrix(camera->GetViewMatrix());
	matricesConstantBuffer->SetProjectionMatrix(camera->GetProjectionMatrix());
	lightConstantBuffer->SetCameraPosition(camera->GetPosition());

	// Render the model using the color shader.
	if (!shader->Render(deviceContext.get(), renderable->GetIndexCount(), texture->GetTexture()))
	{
		return;
	}

	D3D11Controller::Present();
}

LRESULT D3D11RenderController::MessageProcecure(__in HWND hWnd, __in unsigned int msg, __in WPARAM wParam, __in LPARAM lParam)
{
	if (camera)
	{
		camera->MessageProcecure(hWnd, msg, wParam, lParam);
	}

	return D3D11Controller::MessageProcecure(hWnd, msg, wParam, lParam);
}

bool D3D11RenderController::OnResized(__in unsigned int width, __in unsigned int height)
{
	D3D11Controller::OnResized(width, height);

	if (camera)
	{
		camera->SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
	}

	return true;
}
