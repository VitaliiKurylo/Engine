#pragma once

#include "AbstractWindowController.h"

class ENGINE_API WindowController : public AbstractWindowController
{
public:
	WindowController(__in HINSTANCE hInstance);
	virtual ~WindowController(void);

	void Update(__in_opt float deltaTime) override;
	void Render(void) override;
};

