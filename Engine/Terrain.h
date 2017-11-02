#pragma once

#include <Windows.h>
#include <comdef.h>

#include <sstream>
#include <vector>
#include <fstream>
#include <new>
#include <numeric>

#include <d3d11_1.h>
#include <DirectXColors.h>
#include <DirectXMath.h>

#include "Memory.h"
#include "AbstractModel.h"

using namespace DirectX;
using namespace std;

class Terrain : public AbstractModel
{
public:
	Terrain(__in_z const wchar_t* filename);
	virtual ~Terrain();

	bool Init(__in ID3D11Device* device);

protected:
	const wchar_t* heightMapFilename;
	long terrainWidth;
	long terrainHeight;
	vector<VERTEX_DESC> heightMap;

protected:
	bool LoadHeightMap(__in_z const wchar_t* filename);
	bool CalculateNormals(void);
};