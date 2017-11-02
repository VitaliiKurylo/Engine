#pragma once

#define WIN32_LEAN_AND_MEAN

#ifdef _API_EXPORT
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include <Windows.h>
#include <sstream>
#include <string>

using namespace std;

class ENGINE_API AbstractWindowController
{
public:
	virtual ~AbstractWindowController(void);

	int Run(void);

	virtual bool Init(void);
	virtual void Update(__in_opt float deltaTime) = 0;
	virtual void Render(void) = 0;
	virtual LRESULT MessageProcecure(__in HWND hWnd, __in unsigned int msg, __in WPARAM wParam, __in LPARAM lParam);

	virtual bool OnResized(unsigned int width, unsigned int height);

private:
	static LRESULT CALLBACK InternalMessageProcecure(__in HWND hWnd, __in unsigned int msg, __in WPARAM wParam, __in LPARAM lParam)
	{
		auto pWindowController = reinterpret_cast<AbstractWindowController*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		if (pWindowController)
			return pWindowController->MessageProcecure(hWnd, msg, wParam, lParam);

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

protected:
	AbstractWindowController(__in HINSTANCE hInstance);

	HWND appWindow;
	HINSTANCE appInstance;
	unsigned int clientWidth;
	unsigned int clientHeight;
	wchar_t* appTitle;
	wchar_t* appClassName;
	unsigned long windowStyle; 
};

