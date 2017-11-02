#include "AbstractWindowController.h"

AbstractWindowController::AbstractWindowController(__in HINSTANCE hInstance) : appInstance(hInstance),
appWindow(nullptr),
clientWidth(1920),
clientHeight(1080),
appTitle(L"D3D11 Engine"),
appClassName(L"DXENGINEWNDCLASS"),
windowStyle(WS_OVERLAPPEDWINDOW)
{
}

/// <summary>
/// Finalizes an instance of the <see cref="AbstractWindowController"/> class.
/// </summary>
AbstractWindowController::~AbstractWindowController(void)
{
	if (appWindow)
	{
		DestroyWindow(appWindow);
		appWindow = nullptr;
	}

	if (appInstance)
	{
		UnregisterClass(appClassName, appInstance);
		appInstance = nullptr;
	}

	wostringstream debugStream;
	debugStream << "~AbstractWindowController called" << endl;
	OutputDebugString(debugStream.str().c_str());
}

/// <summary>
/// Runs this instance.
/// </summary>
/// <returns></returns>
int AbstractWindowController::Run(void)
{
	MSG message = { 0 };

	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			Update(0);
			Render();
		}
	}

	return static_cast<int>(message.wParam);
}

/// <summary>
/// Initializes this instance.
/// </summary>
/// <returns></returns>
bool AbstractWindowController::Init(void)
{
	WNDCLASSEX wndClassEx = {};

	wndClassEx.cbSize = sizeof(WNDCLASSEX);
	wndClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClassEx.hInstance = appInstance;
	wndClassEx.lpfnWndProc = &InternalMessageProcecure;
	wndClassEx.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndClassEx.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wndClassEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClassEx.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wndClassEx.lpszMenuName = nullptr;
	wndClassEx.lpszClassName = appClassName;

	if (!RegisterClassEx(&wndClassEx))
	{
		OutputDebugString(L"\nFailed to create window class\n");
		return false;
	}

	RECT clientRect = { 0, 0, static_cast<long>(clientWidth), static_cast<long>(clientHeight) };

	AdjustWindowRect(&clientRect, windowStyle, FALSE);

	auto windowWidth = clientRect.right - clientRect.left;
	auto windowHeight = clientRect.bottom - clientRect.top;
	auto windowLeft = GetSystemMetrics(SM_CXSCREEN) / 2 - windowWidth / 2;
	auto windowTop = GetSystemMetrics(SM_CYSCREEN) / 2 - windowHeight / 2;

	appWindow = CreateWindowEx(WS_EX_APPWINDOW, appClassName, appTitle, windowStyle,
		windowLeft, windowTop, windowWidth, windowHeight,
		nullptr, nullptr, appInstance, nullptr);

	if (appWindow)
	{
		SetWindowLongPtr(appWindow, GWLP_USERDATA, reinterpret_cast<ULONG_PTR>(this));
		ShowWindow(appWindow, SW_SHOW);
		UpdateWindow(appWindow);
		return true;
	}
	else
	{
		OutputDebugString(L"\nFailed to create window\n");
		return false;
	}
}

/// <summary>
/// Called when [resized].
/// </summary>
/// <param name="width">The width.</param>
/// <param name="height">The height.</param>
/// <returns></returns>
bool AbstractWindowController::OnResized(__in unsigned int width, __in unsigned int height)
{
	return true;
}

/// <summary>
/// MSGs the proc.
/// </summary>
/// <param name="hWnd">The h WND.</param>
/// <param name="msg">The MSG.</param>
/// <param name="wParam">The w parameter.</param>
/// <param name="lParam">The l parameter.</param>
/// <returns></returns>
LRESULT AbstractWindowController::MessageProcecure(__in HWND hWnd, __in unsigned int msg, __in WPARAM wParam, __in LPARAM lParam)
{
	PAINTSTRUCT lpPaint = {};

	switch (msg)
	{
	case WM_PAINT:
	{
		auto deviceContext = BeginPaint(hWnd, &lpPaint);
		EndPaint(hWnd, &lpPaint);
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	case WM_SIZE:
	{
		OnResized(LOWORD(lParam), HIWORD(lParam));
	}
	break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}