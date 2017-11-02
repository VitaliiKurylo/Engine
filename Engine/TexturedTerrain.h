#pragma once

#include "Terrain.h"

class TexturedTerrain : public Terrain
{
public:
	TexturedTerrain(__in_z const wchar_t* filename);
	virtual ~TexturedTerrain(void);

	bool Init(__in ID3D11Device* device) override;

protected:
	void CalculateTextureCoordinates(void);
};

