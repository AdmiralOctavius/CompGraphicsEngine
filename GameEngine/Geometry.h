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

		VertexPositionColor vertices[6]
		{
			{XMFLOAT3(0, 0, 0),		XMFLOAT4(1, 0, 0, 1)},//RGBA
			{XMFLOAT3(0, .5f, 0),	XMFLOAT4(0, 0, 1, 1)},
			{XMFLOAT3(.5f, 0, 0),	XMFLOAT4(0, 1, 0, 1)},

			{XMFLOAT3(.5f, .5f, .5f),	XMFLOAT4(1, 1, 0, 1)},
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
		//VertexPositionColor vertex2;
		//vertex2.color = Utility::RandomColor();
		//vertices[1] = vertex2;

		//The reason we only see 1 side of the triangle
		//is Backface Culling 
		//Backface culling is where the GPU automatically discards any 'internal' faces
		//of objects

		VertexPositionColor vertices[6]
		{
			{XMFLOAT3(0, 0, 0),		XMFLOAT4(1, 0, 0, 1)},//RGBA
			{XMFLOAT3(0, .5f, 0),	XMFLOAT4(0, 0, 1, 1)},
			{XMFLOAT3(.5f, 0, 0),	XMFLOAT4(0, 1, 0, 1)},
			{XMFLOAT3(.5f, .5f, .5f),	XMFLOAT4(1, 1, 0, 1)},
			
		};

		UINT indice[6] = { 0,1,2,
						1,2,3 };
		mesh->numVertices = 4;
		mesh->numIndices = 6;
		RenderManager::CreateVertexBuffer(mesh->numVertices,
			vertices, mesh->vertexBuffer, device);
		RenderManager::CreateIndexBuffer(indices,mesh->indexBuffer,device);
	}

	
	template<class T>
	static void CreatePyramid(Mesh<T>* mesh, ID3D11Device* device)
	{
		//VertexPositionColor vertex2;
		//vertex2.color = Utility::RandomColor();
		//vertices[1] = vertex2;

		//The reason we only see 1 side of the triangle
		//is Backface Culling 
		//Backface culling is where the GPU automatically discards any 'internal' faces
		//of objects

		VertexPositionColor vertices[12]
		{
			//This is one triangle
			{XMFLOAT3(1, 0, -0.5f),	XMFLOAT4(0, 0, 1, 1)},
			{XMFLOAT3(-1, 0, -0.5f),	XMFLOAT4(0, 1, 0, 1)},
			{XMFLOAT3(0, 1, 0),		XMFLOAT4(1, 0, 0, 1)},//RGBA

			//This is the second triangle
			{XMFLOAT3(-1, 0, -0.5f),	XMFLOAT4(1, 0, 1, 1)},
			{XMFLOAT3(0, 0, 1.5f),	XMFLOAT4(1, 0, 1, 1)},
			{XMFLOAT3(0, 1, 0),	XMFLOAT4(1, 0, 1, 1)},

			//This is the third triangle
			{XMFLOAT3(0, 0, 1.5f),	XMFLOAT4(0, 0, 1, 1)},
			{XMFLOAT3(1, 0, -0.5f),	XMFLOAT4(0, 0, 1, 1)},
			{XMFLOAT3(0, 1, 0),	XMFLOAT4(0, 0, 1, 1)},

			//This is the fourth triangle, the bottom
			{XMFLOAT3(-1, 0, -0.5),	XMFLOAT4(1, 1, 0, 1)},
			{XMFLOAT3(1, 0, -0.5),	XMFLOAT4(1, 1, 0, 1)},
			{XMFLOAT3(0, 0, 1.5f),	XMFLOAT4(1, 1, 0, 1)},
		};
		mesh->numVertices = 12;
		RenderManager::CreateVertexBuffer(mesh->numVertices,
			vertices, mesh->vertexBuffer, device);
	}

	template<class T>
	static void CreateBetterSquareMundell(Mesh<T>* mesh, ID3D11Device* device)
	{
		VertexPositionColor vertices[4]
		{
			{XMFLOAT3(0, 0, 0),		XMFLOAT4(1, 0, 0, 1)},//RGBA
			{XMFLOAT3(0, .5f, 0),	XMFLOAT4(0, 0, 1, 1)},
			{XMFLOAT3(.5f, 0, 0),	XMFLOAT4(0, 1, 0, 1)},
			{XMFLOAT3(.5f, .5f, .5f),	XMFLOAT4(1, 1, 0, 1)},
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

	template<class T>
	static void CreateHillsTest_1(Mesh<T>* mesh, ID3D11Device* device, float gridWidth, float gridDepth, int widthVertices, int depthVertices, bool randomHeight) {
		
		//gridWidth = float width
		//gridDepth = float depth
		//widthVertices = uint m
		//depthVertices = unit n

		if (randomHeight) {
			srand(time(0));
		}
		vector<VertexPositionColor> vertices;
		//VertexPositionColor vertices;
		

		//This should give us the amount of vertices we need
		int vertexCount = widthVertices * depthVertices;
		//This should be the amount of faces we need
		int faceCount = (widthVertices - 1) * (depthVertices - 1) * 2; 

		//More data for grid generation
		float halfWidth = 0.5f * gridWidth;
		float halfDepth = 0.5f * gridDepth;

		//Cell spacing values
		float dx = gridWidth / (depthVertices - 1);
		float dz = gridDepth / (widthVertices - 1);

		//Unsure what math this is for
		//float du = 1.0f / (depthVertices - 1);
		//float dv = 1.0f / (widthVertices - 1);

		//Set the size of the vertices vector
		vertices.resize(vertexCount);

		float randSeed;
		randSeed = rand() % 100;
		randSeed = randSeed / 1000;

		//First loop along the width of the grid
		for (int i = 0; i < widthVertices; ++i) {

			//Position of a vertex along the z axis?
			float z = halfDepth - i*dz;

			//Second loop along the depth
			for (int j = 0; j < depthVertices; ++j) {

				//Position of our X value of the vertex
				float x = -halfWidth + j * dx;

				
				float y;
				if (randomHeight) {
					

					y = randSeed * (z * sinf(0.05f * x) + x * cosf(0.05f * z));
					//cout << "Random height for vertex is: " << y << "\n";
					//printf("Random height for vertex is: %d \n",y);
				}
				else {
					y = 0.0f;
				}

				//sets the vertex position
				vertices.at(i * depthVertices + j).position = XMFLOAT3(x, y, z);

				//Sets color
				if (y > 10) {
					vertices.at(i * depthVertices + j).color = XMFLOAT4(1, 1, 1, 1);
					//cout << "Got here";
				}
				else if (y > 5) {
					
					vertices.at(i * depthVertices + j).color = XMFLOAT4(0.38f, 1, 0.56f, 1);
					
				}
				else if (y < -10) {
					vertices.at(i * depthVertices + j).color = XMFLOAT4(0.4f, 0.2f, 0, 1);
					//cout << "Are all vertex brown";
				}
				else if (y < -5) {
					vertices.at(i * depthVertices + j).color = XMFLOAT4(1, 0.74f, 0.38f, 1);

				}
				else {
					/*if (i * depthVertices + j == 10) {
						vertices.at(i * depthVertices + j).color = XMFLOAT4(1, 0, 0, 1);
						cout << "At Point 10";
					}*/
					//else {
						vertices.at(i * depthVertices + j).color = XMFLOAT4(0, 0.607f, 0.2f, 1);

					//}
				}

			}
		}

		//mesh->numIndices = faceCount * 3;
		////This should give us the amount of vertices we need
		//int vertexCount = widthVertices * depthVertices;
		//This should be the amount of faces we need
		//int faceCount = (widthVertices - 1) * (depthVertices - 1) * 2;

		mesh->numIndices = faceCount * 3;
		vector <UINT> indices;
		indices.resize(faceCount * 3);
		//Mesh indices
		int k = 0;
		for (int i = 0; i < widthVertices - 1; ++i) {
			for (int j = 0; j < depthVertices - 1; ++j) {
				indices[k] = i * depthVertices + j;
				indices[k+1] = i * depthVertices + j + 1;
				indices[k+2] = (i + 1) * depthVertices + j;
				indices[k+3] = (i + 1) * depthVertices + j;
				indices[k+4] = i * depthVertices + j + 1;
				indices[k+5] = (i + 1) * depthVertices + j +1;

				k += 6;
			}
		}

		mesh->numVertices = vertexCount;
		mesh->numIndices = faceCount * 3;
		RenderManager::CreateVertexBuffer(vertices.size(), vertices.data(), mesh->vertexBuffer, device);
		//RenderManager::CreateVertexBuffer()
		RenderManager::CreateIndexBuffer(indices.size(), indices.data(), mesh->indexBuffer, device);
		//enderManager::CreateIndexBuffer()
	}

}