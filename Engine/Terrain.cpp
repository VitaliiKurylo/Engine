#include "Terrain.h"

Terrain::Terrain(__in_z const wchar_t* filename) : heightMapFilename(filename),
terrainWidth(0),
terrainHeight(0)
{
}

Terrain::~Terrain()
{
}

bool Terrain::Init(__in ID3D11Device* device)
{
	using namespace std;

	assert(device);

	try
	{
		if (LoadHeightMap(heightMapFilename))
		{
			if (heightMap.size() > 0)
			{
				CalculateNormals();

				// Create the vertex array.
				vector<VERTEX_DESC> vertices;

				long bottomLeft, bottomRight, upperLeft, upperRight;
				for (auto row = 0; row < (terrainHeight - 1); row++)
				{
					for (auto column = 0; column < (terrainWidth - 1); column++)
					{
						bottomLeft = terrainHeight * row + column;
						bottomRight = terrainHeight * row + column + 1;
						upperLeft = terrainHeight * (row + 1) + column;
						upperRight = terrainHeight * (row + 1) + column + 1;

						vertices.push_back(heightMap[upperLeft]);
						vertices.push_back(heightMap[upperRight]);
						vertices.push_back(heightMap[bottomLeft]);
						vertices.push_back(heightMap[bottomLeft]);
						vertices.push_back(heightMap[upperRight]);
						vertices.push_back(heightMap[bottomRight]);
					}
				}

				// Create the index array.
				vector<unsigned long> indices((terrainHeight - 1) * (terrainWidth - 1) * 6);
				iota(indices.begin(), indices.end(), 0);

				_vertexCount = static_cast<unsigned int>(vertices.size());
				_indexCount = static_cast<unsigned int>(indices.size());

				return AbstractModel::InitBuffers(device, vertices.data(), indices.data());
			}
		}
	}
	catch (const std::bad_alloc& exception)
	{
		wostringstream exceptionStream;
		exceptionStream << L"Failed to allocate memory: " << exception.what() << endl;

		OutputDebugString(exceptionStream.str().c_str());

		return false;
	}

	return false;
}

bool Terrain::LoadHeightMap(__in_z const wchar_t* filename)
{
	using namespace std;

	assert(filename);

	ifstream fin;
	fin.exceptions(fin.exceptions() | ios::failbit);

	try
	{
		fin.open(filename, ios::binary);
		fin.seekg(0, ios::beg);

		BITMAPFILEHEADER bitmapFileHeader = {};
		fin.read(reinterpret_cast<char*>(&bitmapFileHeader), sizeof(BITMAPFILEHEADER));

		BITMAPINFOHEADER bitmapInfoHeader = {};
		fin.read(reinterpret_cast<char*>(&bitmapInfoHeader), sizeof(BITMAPINFOHEADER));

		vector<unsigned char> rawImageData((terrainWidth = bitmapInfoHeader.biWidth) * (terrainHeight = bitmapInfoHeader.biHeight) * 3);

		fin.seekg(bitmapFileHeader.bfOffBits, ios::beg);
		fin.read(reinterpret_cast<char*>(rawImageData.data()), rawImageData.size());

		fin.close();

		heightMap.resize(terrainWidth * terrainHeight);

		auto imageIndex = 0;
		for (auto row = 0; row < terrainHeight; row++)
		{
			for (auto column = 0; column < terrainWidth; column++)
			{
				heightMap[(terrainHeight * row) + column].Position = { static_cast<float>(column), rawImageData[imageIndex] / 15.0f, static_cast<float>(row) };
				heightMap[(terrainHeight * row) + column].Color = XMFLOAT4(Colors::DarkGreen);
				imageIndex += 3;
			}
		}

		return true;
	}
	catch (const ios_base::failure& exception)
	{
		fin.close();

		wostringstream exceptionStream;
		exceptionStream << L"Failed to read file: " << filename << endl;
		exceptionStream << L"Exception: " << exception.what() << endl;

		OutputDebugString(exceptionStream.str().c_str());

		return false;
	}
}

bool Terrain::CalculateNormals(void)
{
	vector<XMVECTOR> normals;

	XMVECTOR vector1 = {};
	XMVECTOR vector2 = {};
	XMVECTOR vector3 = {};

	// Go through all the faces in the mesh and calculate their normals.
	for (auto row = 0; row < (terrainHeight - 1); row++)
	{
		for (auto column = 0; column < (terrainWidth - 1); column++)
		{
			// Get three vertices from the face.
			vector1 = XMLoadFloat3(&heightMap[terrainHeight * row + column].Position);
			vector2 = XMLoadFloat3(&heightMap[terrainHeight * row + column + 1].Position);
			vector3 = XMLoadFloat3(&heightMap[terrainHeight * (row + 1) + column].Position);

			// Calculate the two vectors for this face.
			vector1 -= vector3;
			vector2 = vector3 - vector2;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals.push_back(XMVector3Cross(vector1, vector2));
		}
	}

	XMVECTOR sum = {};
	auto count = 0;

	// Now go through all the vertices and take an average of each face normal that the vertex touches to get the averaged normal for that vertex.
	for (auto row = 0; row < terrainHeight; row++)
	{
		for (auto column = 0; column < terrainWidth; column++)
		{
			sum = {};
			count = 0;

			// Bottom left face.
			if ((column - 1) >= 0 && (row - 1) >= 0)
			{
				sum += normals[(terrainHeight - 1) * (row - 1) + column - 1];
				count++;
			}

			// Bottom right face.
			if (column < (terrainWidth - 1) && (row - 1) >= 0)
			{
				sum += normals[(terrainHeight - 1) * (row - 1) + column];
				count++;
			}

			// Upper left face.
			if ((column - 1) >= 0 && row < (terrainHeight - 1))
			{
				sum += normals[(terrainHeight - 1) * row + column - 1];
				count++;
			}

			// Upper right face.
			if (column < (terrainWidth - 1) && row < (terrainHeight - 1))
			{
				sum += normals[(terrainHeight - 1) * row + column];
				count++;
			}

			XMStoreFloat3(&heightMap[terrainHeight * row + column].Normal, XMVector3Normalize(sum / static_cast<float>(count)));
		}
	}

	return true;
}