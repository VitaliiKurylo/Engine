#include "TexturedTerrain.h"

TexturedTerrain::TexturedTerrain(__in_z const wchar_t* filename) : Terrain(filename)
{
}

TexturedTerrain::~TexturedTerrain(void)
{
}

bool TexturedTerrain::Init(__in ID3D11Device* device)
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
				CalculateTextureCoordinates();

				// Create the vertex array.
				vector<VERTEX_DESC> vertices;

				VERTEX_DESC currentVertex = {};
				long bottomLeft, bottomRight, upperLeft, upperRight;
				for (auto row = 0; row < (terrainHeight - 1); row++)
				{
					for (auto column = 0; column < (terrainWidth - 1); column++)
					{
						bottomLeft = terrainHeight * row + column;
						bottomRight = terrainHeight * row + column + 1;
						upperLeft = terrainHeight * (row + 1) + column;
						upperRight = terrainHeight * (row + 1) + column + 1;

						currentVertex = heightMap[upperLeft];
						currentVertex.Texture.y = (currentVertex.Texture.y == 1.0f) ? 0.0f : currentVertex.Texture.y;
						vertices.push_back(currentVertex);

						currentVertex = heightMap[upperRight];
						currentVertex.Texture.x = (currentVertex.Texture.x == 0.0f) ? 1.0f : currentVertex.Texture.x;
						currentVertex.Texture.y = (currentVertex.Texture.y == 1.0f) ? 0.0f : currentVertex.Texture.y;
						vertices.push_back(currentVertex);

						vertices.push_back(heightMap[bottomLeft]);
						vertices.push_back(heightMap[bottomLeft]);


						currentVertex = heightMap[upperRight];
						currentVertex.Texture.x = (currentVertex.Texture.x == 0.0f) ? 1.0f : currentVertex.Texture.x;
						currentVertex.Texture.y = (currentVertex.Texture.y == 1.0f) ? 0.0f : currentVertex.Texture.y;
						vertices.push_back(currentVertex);

						currentVertex = heightMap[bottomRight];
						currentVertex.Texture.x = (currentVertex.Texture.x == 0.0f) ? 1.0f : currentVertex.Texture.x;
						vertices.push_back(currentVertex);
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

void TexturedTerrain::CalculateTextureCoordinates(void)
{
	const auto textureRepeat = 4.0f;

	// Calculate how much to increment the texture coordinates by.
	auto incrementValue = textureRepeat / static_cast<float>(terrainWidth);

	// Calculate how many times to repeat the texture.
	auto incrementCount = terrainWidth / textureRepeat;

	// Initialize the tu and tv coordinate values.
	auto tuCoordinate = 0.0f;
	auto tvCoordinate = 1.0f;

	// Initialize the tu and tv coordinate indexes.
	auto tuCount = 0;
	auto tvCount = 0;

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for (auto row = 0; row < terrainHeight; row++)
	{
		for (auto column = 0; column < terrainWidth; column++)
		{
			// Store the texture coordinate in the height map.
			heightMap[(terrainHeight * row) + column].Texture = { tuCoordinate, tvCoordinate };

			// Increment the tu texture coordinate by the increment value and increment the index by one.
			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture and if so then start at the beginning again.
			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// Increment the tv texture coordinate by the increment value and increment the index by one.
		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture and if so then start at the bottom again.
		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}
}