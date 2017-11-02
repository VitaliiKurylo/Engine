#include <Windows.h>

/// <summary>
/// DLLs the main.
/// </summary>
/// <param name="hinstDLL">The hinst DLL.</param>
/// <param name="fdwReason">The FDW reason.</param>
/// <param name="lpvReserved">The LPV reserved.</param>
/// <returns></returns>
bool WINAPI DllMain(__in HINSTANCE hinstDLL, __in DWORD fdwReason, __in LPVOID lpvReserved)
{
	return true;
}