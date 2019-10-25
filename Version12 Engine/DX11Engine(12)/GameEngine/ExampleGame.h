#pragma once

#include <DirectXMath.h>
#include <d3dcompiler.h> 
#include <vector>
#include <unordered_map>//Hash Table

#include "Mesh.h"
#include "GameObject.h"
#include "Camera.h"
#include "BaseGame.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "Light.h"
#include "ConstantBufferDefinitions.h"
#include "VertexDefinitions.h"
#include "PostProcess.h"
#include "Sprite.h"

/* This represents a particular game and its entities, meshes, shaders, etc.  Just remember that the Program
actually starts in main.cpp and the Game Loop is defined in BaseGame.cpp */
class ExampleGame final : public BaseGame
{
public:
	ExampleGame(HINSTANCE instance);
 	~ExampleGame();
	bool Init() override;
	void GetInput(float dt) override;
	void Update(float dt) override;
	void Clear() override;
	void Draw() override;
	void OnResize() override;
	void OnFocus() override;

private:
	//Assets
	SpriteFont *spriteFont = nullptr;
	SpriteBatch *spriteBatch = nullptr;
	//Hash Tables
	unordered_map<string, ID3D11ShaderResourceView*> textureMap;
	unordered_map<string, ID3D11SamplerState*> samplerMap;
	unordered_map<string, ID3D11InputLayout*> inputLayoutMap;
	unordered_map<string, ID3D11VertexShader*> vShaderMap;
	unordered_map<string, ID3D11PixelShader*> pShaderMap;
	unordered_map<string, Material> materialMap;
	unordered_map<string, BaseMesh*> meshMap;
	Mesh<VertexPositionColor> triangleMesh;
	Mesh<VertexPositionColor> squareMesh;
	Mesh<VertexPositionColor> betterSquareMesh;

	//Render States and Objects
	ID3D11RasterizerState* rasterizerState = nullptr;
	ID3D11RasterizerState *wireFrameState = nullptr;
	ID3D11BlendState *standardBlend = nullptr, *noBlend = nullptr;
	ID3D11DepthStencilState *standardDepthState = nullptr, *skyDepthState = nullptr;
	SimpleConstantBuffer gameSimpleCBuffer;//constant buffer FOR Game
	ID3D11Buffer *gpuSimpleCBuffer = nullptr;//constant buffer FOR GPU
	Camera camera;
	DirectionalLight sun;

	//Other Settings
	float clearcolor[4];
	float ambient = .025f;
	int curSampler = 3;
	bool wireframe = false;
	wstring filterText = L"Anisotropic Filtering";
	
	//GameObjects
	vector<GameObject> opaqueObjects;//For drawing in one line
	GameObject sky;
	GameObject triangle;
	GameObject square;
	GameObject betterSquare;
	GameObject* table;
	GameObject* ground;
	GameObject* texturedSquare;
};