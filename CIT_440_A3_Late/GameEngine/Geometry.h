#pragma once

#include "Mesh.h"
#include "RenderManager.h"

namespace Geometry
{
	template<class T>
	static void CreateTriangle(Mesh<T>* mesh, ID3D11Device* device)
	{
		//VertexPositionColor vertex2;
		//vertex2.color = Utility::RandomColor();
		//vertices[1] = vertex2;

		//The reason we only see 1 side of the triangle
		//is Backface Culling 
		//Backface culling is where the GPU automatically discards any 'internal' faces
		//of objects

		//VertexPositionColor vertices[3]
		//{
		//	{XMFLOAT3(0, 0, 0),		XMFLOAT4(1, 0, 0, 1)},//RGBA
		//	{XMFLOAT3(0, .5f, 0),	XMFLOAT4(0, 0, 1, 1)},
		//	{XMFLOAT3(.5f, 0, 0),	XMFLOAT4(0, 1, 0, 1)},
		//};

		vector<VertexPositionColor> vertices;
		vertices.reserve(3);
		vertices.push_back({ XMFLOAT3(0, 0, 0),		XMFLOAT4(1, 0, 0, 1) });
		vertices.push_back({ XMFLOAT3(0, .5f, 0),	XMFLOAT4(0, 0, 1, 1) });
		vertices.push_back({ XMFLOAT3(.5f, 0, 0),	XMFLOAT4(0, 1, 0, 1) });

		//vertices.resize(3);
		//vertices[0].position = XMFLOAT3(0, .5f, 0);

		mesh->numVertices = 3;
		RenderManager::CreateVertexBuffer(mesh->numVertices, 
			vertices.data(), mesh->vertexBuffer, device);

		//or
		//RenderManager::CreateVertexBuffer(vertices, mesh->vertexBuffer, device);
	}
	template<class T>
		static void CreateSquare(Mesh<T>* mesh, ID3D11Device* device)
	{
		VertexPositionColor vertices[6]
		{
			{XMFLOAT3(0, 0, 0),		XMFLOAT4(1, 0, 0, 1)},//RGBA
			{XMFLOAT3(0, .5f, 0),	XMFLOAT4(0, 0, 1, 1)},
			{XMFLOAT3(.5f, 0, 0),	XMFLOAT4(0, 1, 0, 1)},

			{XMFLOAT3(.5f, .5f, 0),	XMFLOAT4(1, 1, 0, 1)},
			{XMFLOAT3(.5f, 0, 0),	XMFLOAT4(1, 1, 0, 1)},
			{XMFLOAT3(0, .5f, 0),	XMFLOAT4(1, 1, 0, 1)},
		};
		mesh->numVertices = 6;
		RenderManager::CreateVertexBuffer(mesh->numVertices,
			vertices, mesh->vertexBuffer, device);
	}

	template<class T>
	static void CreateBetterSquare(Mesh<T>* mesh, ID3D11Device* device)
	{
		VertexPositionColor vertices[4]
		{
			{XMFLOAT3(0, 0, 0),		XMFLOAT4(1, 0, 0, 1)},//RGBA
			{XMFLOAT3(0, .5f, 0),	XMFLOAT4(0, 0, 1, 1)},
			{XMFLOAT3(.5f, 0, 0),	XMFLOAT4(0, 1, 0, 1)},
			{XMFLOAT3(.5f, .5f, 0),	XMFLOAT4(1, 1, 0, 1)},
		};

		//An index buffer lets us reuse vertices instead of
		//having duplicate vertices with the same position
		UINT indices[6] = { 0, 1, 2, //triangle 1
							1, 3, 2 };//triangle 2

		mesh->numVertices = 4;
		mesh->numIndices = 6;
		RenderManager::CreateVertexBuffer(mesh->numVertices,
			vertices, mesh->vertexBuffer, device);
		RenderManager::CreateIndexBuffer(6, indices, mesh->indexBuffer, device);
	}
}