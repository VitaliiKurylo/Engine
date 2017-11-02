#include "WindowController.h"

/// <summary>
/// Initializes a new instance of the <see cref="WindowController"/> class.
/// </summary>
/// <param name="hInstance">The h instance.</param>
WindowController::WindowController(__in HINSTANCE hInstance) : AbstractWindowController(hInstance)
{
}

/// <summary>
/// Finalizes an instance of the <see cref="WindowController"/> class.
/// </summary>
WindowController::~WindowController(void)
{
	wostringstream debugStream;
	debugStream << "~WindowController called" << endl;
	OutputDebugString(debugStream.str().c_str());
}

/// <summary>
/// Updates the specified delta time.
/// </summary>
/// <param name="deltaTime">The delta time.</param>
void WindowController::Update(__in_opt float deltaTime)
{
}

/// <summary>
/// Renders the specified delta time.
/// </summary>
/// <param name="deltaTime">The delta time.</param>
void WindowController::Render(void)
{
}