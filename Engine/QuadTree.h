#pragma once

#include "AbstractModel.h"

class QuadTree
{
public:
	QuadTree();
	virtual ~QuadTree();

	bool Init(__in ID3D11Device* device);
	void Render(__in ID3D11DeviceContext* deviceContext);
	unsigned int GetIndexCount(void) const { return indexCount; }

protected:
	struct QuadTreeNode
	{
		float X;
		float Z;
		float Width;
		int TriangleCount;
		ID3D11Buffer* VertexBuffer;
		ID3D11Buffer* IndexBuffer;
		QuadTreeNode* Children[4];
	};

protected:
	QuadTreeNode* Root;
	int triangleCount;
	int indexCount;
};