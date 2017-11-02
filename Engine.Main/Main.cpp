#include "D3D11RenderController.h"

#pragma comment(lib, "Engine.lib")

int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	D3D11RenderController controller(hInstance);
	if(controller.Init())
	{
		return controller.Run();
	}
	else
	{
		return 0;
	}
}