#pragma once

#include "Mesh.h"
#include "RenderManager.h"

namespace Geometry
{
	template<class T>
	static void CreateTriangle(Mesh<T>* mesh, ID3D11Device* device) {

		VertexPositionColor vertices[3]{
			{XMFLOAT3(0, 0, 0),	XMFLOAT4(1,0,0,0)},
			{XMFLOAT3(.5f, 0, 0),	XMFLOAT4(0,0,1,1)},
			{XMFLOAT3(0, .5f, 0),		XMFLOAT4(1,1,0,1)}
		};
		mesh->numVertices = 3;
		RenderManager::CreateVertexBuffer(vertices, mesh->vertexBuffer, device);

	}
}