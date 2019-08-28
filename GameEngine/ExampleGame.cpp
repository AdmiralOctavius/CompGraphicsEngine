#pragma region Includes and Namespaces
#include <string>

#include "ExampleGame.h"
#include "VertexDefinitions.h"
#include "Geometry.h"
#include "ModelImporter.h"
#include "Mesh.h"

using namespace std;
using namespace DirectX;
#pragma endregion

#pragma region Setup
//The Game() constructor
ExampleGame::ExampleGame(HINSTANCE instance)
	: BaseGame(instance)
{
	windowCaption = L"ExampleGame ";
}

//Init() is for initializing the game Objects. 
bool ExampleGame::Init()
{
	if (!BaseGame::Init())
		return false;	 

	srand((UINT)time(0));//Seed Random so its different every run.
	 
	//background color
	clearcolor[0] = 0;//Red
	clearcolor[1] = 0;//Green
	clearcolor[2] = 0;//Blue
	clearcolor[3] = 1;//Alpha

	//Create Font and SpriteBatch for drawing text on the screen
	//Spritebatch/Font comes from the DirectX Tool Kit, which is Microsoft provided code
	spriteBatch = new SpriteBatch(deviceContext);
	spriteFont = new SpriteFont(device, L"..\\Assets\\Fonts\\segoe12.spritefont"); 
	spriteFont->SetLineSpacing(0);

	//Setup constant buffers
	RenderManager::CreateConstantBuffer<SimpleConstantBuffer>(gpuSimpleCBuffer, device);
	
	//Setup Globals so we can conveniently get these commonly used objects
	Globals::Get().SetDevice(device);
	Globals::Get().SetDeviceContext(deviceContext);
	Globals::Get().SetGameCBuffer(&gameSimpleCBuffer);
	Globals::Get().SetGPUCBuffer(gpuSimpleCBuffer);

	//Create states and buffers.
	RenderManager::CreateRasterizerState(rasterizerState, device, D3D11_FILL_SOLID, D3D11_CULL_BACK);
	RenderManager::CreateDepthStencilState(standardDepthState, device);
	RenderManager::CreateDepthStencilState(skyDepthState, device, true, D3D11_COMPARISON_LESS_EQUAL);
	RenderManager::CreateBlendState(noBlend, device, false);
	RenderManager::CreateBlendState(standardBlend, device);

	//Load Textures
	ZeroMemory(&textureMap["blank"], sizeof(ID3D11ShaderResourceView));
	textureMap["sky"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\skybox.dds", device, deviceContext);
	textureMap["groundDiffuse"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\Grass1_Diffuse.jpg", device, deviceContext);
	textureMap["barrelDiffuse"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\BarrelTex.jpg", device, deviceContext);
	textureMap["barrelSpec"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\BarrelSpecMap.jpg", device, deviceContext);
	textureMap["barrelNormal"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\BarrelNormalMap.jpg", device, deviceContext);
	textureMap["kirbyDiffuse"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\KirbyDiffuse1.jpg", device, deviceContext);
	textureMap["kirbySpec"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\KirbySpec2.jpg", device, deviceContext);
	textureMap["kirbyMask"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\KirbyEffectMask.jpg", device, deviceContext);
	textureMap["wellDiffuse"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\WellTex.jpg", device, deviceContext);
	textureMap["wellSpec"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\WellSpecMap.jpg", device, deviceContext);
	textureMap["wellNormal"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\WellNormalMap.jpg", device, deviceContext);

	//Create Samplers
	samplerMap["default"] = RenderManager::CreateSamplerState(device);

	//Load Shaders
	RenderManager::CreateVertexShaderAndInputLayout<VertexPosColNormal>(L"VS_color.cso", "color", vShaderMap, inputLayoutMap, device);
	pShaderMap["color"] = RenderManager::CreatePixelShader(L"PS_color.cso", device);
	RenderManager::CreateVertexShaderAndInputLayout<VertexPosColNormal>(L"VS_light.cso", "lit", vShaderMap, inputLayoutMap, device);
	pShaderMap["lit"] = RenderManager::CreatePixelShader(L"PS_light.cso", device);
	RenderManager::CreateVertexShaderAndInputLayout<VertexPosNormTexture>(L"VS_litTextured.cso", "litTextured", vShaderMap, inputLayoutMap, device);
	RenderManager::CreateVertexShaderAndInputLayout<VertexSky>(L"VS_skybox.cso", "skybox", vShaderMap, inputLayoutMap, device);
	pShaderMap["litTextured"] = RenderManager::CreatePixelShader(L"PS_litTextured.cso", device);
	pShaderMap["skybox"] = RenderManager::CreatePixelShader(L"PS_skybox.cso", device);
	RenderManager::CreateVertexShaderAndInputLayout<VertexStandard>(L"VS_normalmapped.cso", "normalmapping", vShaderMap, inputLayoutMap, device);
	pShaderMap["normalmapping"] = RenderManager::CreatePixelShader(L"PS_normalmapped.cso", device);

	//Setup materials
	Material::Init();//Just allocates memory for textures and samplers. Mostly for optimization; you could do it each time you apply 1 more than before instead

	materialMap["sky"] = Material();
	materialMap["sky"].rasterizerState = rasterizerState;
	materialMap["sky"].depthState = skyDepthState;
	materialMap["sky"].vShader = vShaderMap["skybox"];
	materialMap["sky"].pShader = pShaderMap["skybox"];
	materialMap["sky"].inputLayout = inputLayoutMap["lit"];
	materialMap["sky"].textures.push_back(textureMap["sky"]);
	materialMap["sky"].samplers.push_back(samplerMap["default"]);
	
	materialMap["lit"] = Material();
	materialMap["lit"].rasterizerState = rasterizerState;
	materialMap["lit"].depthState = standardDepthState;
	materialMap["lit"].blendState = noBlend;
	materialMap["lit"].vShader = vShaderMap["lit"];
	materialMap["lit"].inputLayout = inputLayoutMap["lit"];
	materialMap["lit"].pShader = pShaderMap["lit"];

	//Just for demoing different effects
	materialMap["textured"] = Material();
	materialMap["textured"].rasterizerState = rasterizerState;
	materialMap["textured"].depthState = standardDepthState;
	materialMap["textured"].blendState = noBlend;
	materialMap["textured"].vShader = vShaderMap["litTextured"];
	materialMap["textured"].inputLayout = inputLayoutMap["litTextured"];
	materialMap["textured"].pShader = pShaderMap["litTextured"];
	materialMap["textured"].samplers.push_back(samplerMap["default"]);

	materialMap["kirby"] = materialMap["textured"];
	materialMap["kirby"].textures.push_back(textureMap["kirbyDiffuse"]);
	materialMap["kirby"].textures.push_back(textureMap["kirbySpec"]);
	materialMap["kirby"].textures.push_back(textureMap["kirbyMask"]);
	materialMap["kirby"].textures.push_back(textureMap["sky"]);

	materialMap["barrel"] = materialMap["textured"];
	materialMap["barrel"].textures.push_back(textureMap["barrelDiffuse"]);
	materialMap["barrel"].textures.push_back(textureMap["barrelSpec"]);
	materialMap["barrel"].textures.push_back(textureMap["blank"]);
	materialMap["barrel"].textures.push_back(textureMap["sky"]);

	materialMap["ground"] = materialMap["textured"];
	materialMap["ground"].textures.push_back(textureMap["groundDiffuse"]);
	materialMap["ground"].textures.push_back(textureMap["blank"]);
	materialMap["ground"].textures.push_back(textureMap["blank"]);
	materialMap["ground"].textures.push_back(textureMap["blank"]);

	materialMap["well"] = Material();
	materialMap["well"].rasterizerState = rasterizerState;
	materialMap["well"].depthState = standardDepthState;
	materialMap["well"].blendState = noBlend;
	materialMap["well"].vShader = vShaderMap["normalmapping"];
	materialMap["well"].inputLayout = inputLayoutMap["normalmapping"];
	materialMap["well"].pShader = pShaderMap["normalmapping"];
	materialMap["well"].samplers.push_back(samplerMap["default"]);
	materialMap["well"].textures.push_back(textureMap["wellDiffuse"]);
	materialMap["well"].textures.push_back(textureMap["wellSpec"]);
	materialMap["well"].textures.push_back(textureMap["blank"]);
	materialMap["well"].textures.push_back(textureMap["wellNormal"]);
	materialMap["well"].textures.push_back(textureMap["blank"]);

	//Import models
	meshMap["Skybox"] = new Mesh<VertexSky>();
	meshMap["Kirby"] = new Mesh<VertexPosNormTexture>();
	meshMap["Barrel"] = new Mesh<VertexPosNormTexture>();
	meshMap["Ground"] = new Mesh<VertexPosNormTexture>();
	meshMap["Well"] = new Mesh<VertexPosTexNormTan>();

	ModelImporter::LoadFromFile("../Assets/Models/invertedcube.obj", (Mesh<VertexSky>*)meshMap["Skybox"], device);
	ModelImporter::LoadFromFile("../Assets/Models/kirby.obj", (Mesh<VertexPosNormTexture>*)meshMap["Kirby"], device);
	ModelImporter::LoadFromFile("../Assets/Models/barrel.obj", (Mesh<VertexPosNormTexture>*)meshMap["Barrel"], device);
	ModelImporter::LoadFromFile("../Assets/Models/ground.obj", (Mesh<VertexPosNormTexture>*)meshMap["Ground"], device);
	ModelImporter::LoadFromFileWithTangents("../Assets/Models/well.obj", (Mesh<VertexPosTexNormTan>*)meshMap["Well"], device);

	//Setup other game settings as needed
	camera.useSpaceCamera = false;
	camera.position = XMFLOAT3(0, 2, -5);
	camera.forward = XMFLOAT3(0, -2, 1);
	sun.direction = XMFLOAT3(0, -1, 1);
	mouse->InitMouseCallback(gameWindow);//Enable hi-res mouse input
	gameSimpleCBuffer.ambientLightIntensity = ambient;

	//Setup game objects (ground, buildings, characters, etc.)
	sky.mesh = meshMap["Skybox"];
	sky.material = &materialMap["sky"];

	//More efficient to allocate ahead of time rather than growing to size slowly.
	opaqueObjects.reserve(100);

	GameObject ground;
	ground.mesh = meshMap["Ground"];
	ground.material = &materialMap["ground"];
	ground.uvScale = XMFLOAT2(25, 25);
	opaqueObjects.emplace_back(ground);

	GameObject well;
	well.material = &materialMap["well"];
	well.mesh = meshMap["Well"];
	well.position.x -= 5;
	opaqueObjects.emplace_back(well);

	GameObject barrel;
	barrel.material = &materialMap["barrel"];
	barrel.mesh = meshMap["Barrel"];
	opaqueObjects.emplace_back(barrel);

	GameObject kirby;
	kirby.material = &materialMap["kirby"];
	kirby.mesh = meshMap["Kirby"];
	kirby.rotation.x = XMConvertToRadians(90);
	kirby.position.x += 0;
	kirby.position.y += 1.12;

	opaqueObjects.emplace_back(kirby);

	//We must resize once at the beginning of the game to build the initial screen buffers, camera aspect ratio, etc.
	//After that it only gets called if the game window changes size/maximizes/fullscreens, etc.
	OnResize(); 

	return true;
}
#pragma endregion

#pragma region Game Loop
//GetInput is called in BaseGame::Run before Update() and Draw().
void ExampleGame::GetInput(float dt)//dt = deltaTime
{
	if (keyboard->ButtonDown(VK_ESCAPE))
		PostQuitMessage(0); 
} 

//Update is for movement, AI, physics, most gameplay things.
void ExampleGame::Update(float dt)
{
	if (!mouse->IsVisible())
		mouse->CenterCursor();//Do anything that requires mouse position/dx/dy, etc. before this line.
	
	sun.Update();
	camera.Update();
	sky.position = camera.position;
}

//Clear() resets the backbuffer texture so that we are drawing to a blank screen
void ExampleGame::Clear()
{
	Material::Reset();
	deviceContext->ClearRenderTargetView(renderTargetView, clearcolor);
	deviceContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
}

//Draw is where we actually render to the screen
void ExampleGame::Draw()
{	
	Clear();//Clear the buffer (otherwise we'll see what was drawn on it before)
	
	//Setup for drawing
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//every 3 vertices = 1 triangle.
	deviceContext->VSSetConstantBuffers(0, 1, &gpuSimpleCBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &gpuSimpleCBuffer); 

	//Draw opaque objects first
	for (GameObject &g : opaqueObjects)
		g.Draw(); 

	//Then draw background/skybox
	sky.Draw();
		
	//Draw 2D UI/sprites after all 3D game elements
	//Draw text to the screen
	spriteBatch->Begin();
	{
		spriteFont->DrawString(spriteBatch, std::wstring(L"FPS: " + std::to_wstring((int)frameRate)).c_str(), XMFLOAT2(0, 0), XMVectorSet(1, 0, 0, 1));
	}
	spriteBatch->End();//Text doesn't actually get drawn to the screen until here

	//Finally present new image to the screen
	HR(swapChain->Present(vsync, 0));
}	
#pragma endregion

#pragma region Events
//Reacquire devices when the window loses and regains focus
void ExampleGame::OnFocus()
{
	if(mouse != nullptr)
		mouse->onFocus(gameWindow);

	if(keyboard != nullptr)
		keyboard->onFocus(gameWindow);
}

//OnResize is called whenever the user changes the size of the window - update anything that depends on the window size here.
void ExampleGame::OnResize()
{
	BaseGame::OnResize();
	gameSimpleCBuffer.screenSize.x = (float)windowWidth;
	gameSimpleCBuffer.screenSize.y = (float)windowHeight;

	Globals::Get().SetAspectRatio(AspectRatio());
}

//The ~Game() destructor is for releasing and deleting all our objects
//Remember Release is for DirectX objects, and delete is for pointers that use the 'new' keyword
ExampleGame::~ExampleGame()
{
	//Delete our things
	SafeDelete(spriteFont);
	SafeDelete(spriteBatch); 

	SafeDeleteMap(meshMap);

	//Release DirectX Things
	//Reminder: the goal is to see a refcount of 2 when
		//you run in debug mode and look at the output log
		//the last 2 objects are the device and debug object, 
		//they'll release in ~BaseGame
	SafeRelease(rasterizerState);

	SafeRelease(standardDepthState);
	SafeRelease(skyDepthState);

	SafeRelease(standardBlend);
	SafeRelease(noBlend);

	SafeRelease(gpuSimpleCBuffer);

	SafeReleaseMap(textureMap);
	SafeReleaseMap(samplerMap);
	SafeReleaseMap(inputLayoutMap);
	SafeReleaseMap(vShaderMap);
	SafeReleaseMap(pShaderMap);
}
#pragma endregion