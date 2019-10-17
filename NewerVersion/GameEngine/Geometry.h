#pragma once

#include "Mesh.h"
#include "RenderManager.h"

namespace Geometry
{
	template<class T>
	static void CreateGroundPlane(Mesh<T>* mesh, ID3D11Device* device)
	{
		vector<VertexPosTexNormTan> vertices(4);
		
		vertices[0].position = XMFLOAT3(-.5f, 0, .5f);
		vertices[1].position = XMFLOAT3(.5f, 0, .5f);
		vertices[2].position = XMFLOAT3(.5f, 0, -.5f);
		vertices[3].position = XMFLOAT3(-.5f, 0, -.5f);

		vertices[0].normal = 
		vertices[1].normal = 
		vertices[2].normal = 
		vertices[3].normal = XMFLOAT3(0, 1, 0);

		vertices[0].texture = XMFLOAT2(0, 0);
		vertices[1].texture = XMFLOAT2(1, 0);
		vertices[2].texture = XMFLOAT2(1, 1);
		vertices[3].texture = XMFLOAT2(0, 1);
		
		vector<UINT> indices = { 0, 1, 2, //triangle 1
								2, 3, 0 };//triangle 2
		
		//fills in vertices[x].tangent's
		ModelImporter::CalcTangent(vertices, indices);

		mesh->numVertices = 4;
		mesh->numIndices = 6;
		RenderManager::CreateVertexBuffer(vertices, mesh->vertexBuffer, device);
		RenderManager::CreateIndexBuffer(indices, mesh->indexBuffer, device);
	}

	template<class T>
	static void CreatePictureFrame1(Mesh<T>* mesh, ID3D11Device* device)
	{
		vector<VertexPosTexNormTan> vertices(4);

		vertices[0].position = XMFLOAT3(-.5f, .5f, 0);	//Top Left
		vertices[1].position = XMFLOAT3(.5f, .5f, 0);	//Top Right
		vertices[2].position = XMFLOAT3(.5f, -.5f, 0);	//Bottom Right
		vertices[3].position = XMFLOAT3(-.5f, -.5f, 0);	//BottomLeft

		vertices[0].normal =
			vertices[1].normal =
			vertices[2].normal =
			vertices[3].normal = XMFLOAT3(0, 1, 0);

		vertices[0].texture = XMFLOAT2(0, 0);//Top Left
		vertices[1].texture = XMFLOAT2(1, 0);//Top Right
		vertices[2].texture = XMFLOAT2(1, 1);//Bottom Right
		vertices[3].texture = XMFLOAT2(0, 1);//BottomLeft

		vector<UINT> indices = { 0, 1, 2, //triangle 1
								2, 3, 0 };//triangle 2

		//fills in vertices[x].tangent's
		ModelImporter::CalcTangent(vertices, indices);

		mesh->numVertices = 4;
		mesh->numIndices = 6;
		RenderManager::CreateVertexBuffer(vertices, mesh->vertexBuffer, device);
		RenderManager::CreateIndexBuffer(indices, mesh->indexBuffer, device);
	}

	template<class T>
	static void CreatePictureFrame2(Mesh<T>* mesh, ID3D11Device* device)
	{
		vector<VertexPosTexNormTan> vertices(4);

		vertices[0].position = XMFLOAT3(-.5f, .5f, 0);	//Top Left
		vertices[1].position = XMFLOAT3(.5f, .5f, 0);	//Top Right
		vertices[2].position = XMFLOAT3(.5f, -.5f, 0);	//Bottom Right
		vertices[3].position = XMFLOAT3(-.5f, -.5f, 0);	//BottomLeft

		vertices[0].normal =
			vertices[1].normal =
			vertices[2].normal =
			vertices[3].normal = XMFLOAT3(0, 1, 0);

		vertices[0].texture = XMFLOAT2(0, 0);//Top Left
		vertices[1].texture = XMFLOAT2(1, 0);//Top Right
		vertices[2].texture = XMFLOAT2(1, 1);//Bottom Right
		vertices[3].texture = XMFLOAT2(0, 1);//BottomLeft

		vector<UINT> indices = { 0, 1, 2, //triangle 1
								2, 3, 0 };//triangle 2

		//fills in vertices[x].tangent's
		ModelImporter::CalcTangent(vertices, indices);

		mesh->numVertices = 4;
		mesh->numIndices = 6;
		RenderManager::CreateVertexBuffer(vertices, mesh->vertexBuffer, device);
		RenderManager::CreateIndexBuffer(indices, mesh->indexBuffer, device);
	}

	template<class T>
	static void CreateWall(Mesh<T>* mesh, ID3D11Device* device)
	{
		vector<VertexPosTexNormTan> vertices(4);

		vertices[0].position = XMFLOAT3(-.5f, .5f, 0);	//Top Left
		vertices[1].position = XMFLOAT3(.5f, .5f, 0);	//Top Right
		vertices[2].position = XMFLOAT3(.5f, -.5f, 0);	//Bottom Right
		vertices[3].position = XMFLOAT3(-.5f, -.5f, 0);	//BottomLeft

		vertices[0].normal =
			vertices[1].normal =
			vertices[2].normal =
			vertices[3].normal = XMFLOAT3(0, 0, -1);

		vertices[0].texture = XMFLOAT2(0, 0);//Top Left
		vertices[1].texture = XMFLOAT2(1, 0);//Top Right
		vertices[2].texture = XMFLOAT2(1, 1);//Bottom Right
		vertices[3].texture = XMFLOAT2(0, 1);//BottomLeft

		vector<UINT> indices = { 0, 1, 2, //triangle 1
								2, 3, 0 };//triangle 2

		//fills in vertices[x].tangent's
		ModelImporter::CalcTangent(vertices, indices);

		mesh->numVertices = 4;
		mesh->numIndices = 6;
		RenderManager::CreateVertexBuffer(vertices, mesh->vertexBuffer, device);
		RenderManager::CreateIndexBuffer(indices, mesh->indexBuffer, device);
	}
}