#pragma region Includes and Namespaces
#include <string>
#include <cmath>

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
	textureMap["blank"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\blank.jpg", device, deviceContext);
	textureMap["candle"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\candle.jpg", device, deviceContext);
	textureMap["agreatman"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\agreatman.jpg", device, deviceContext);
	textureMap["scream"] = RenderManager::CreateTexture(L"..\\Assets\\Images\\scream.jpg", device, deviceContext);

	//Create Samplers
	samplerMap["default"] = RenderManager::CreateSamplerState(device);
	
	//Load Shaders
	RenderManager::CreateVertexShaderAndInputLayout<VertexPosTexNormTan>
		(L"VS_inclass.cso", "inclass", vShaderMap, inputLayoutMap, device);
	pShaderMap["inclass"] = RenderManager::CreatePixelShader(L"PS_inclass.cso", device);

	//Setup materials
	Material::Init();

	materialMap["inclass"] = Material();
	materialMap["inclass"].rasterizerState = rasterizerState;
	materialMap["inclass"].depthState = standardDepthState;
	materialMap["inclass"].blendState = noBlend;
	materialMap["inclass"].vShader = vShaderMap["inclass"];
	materialMap["inclass"].inputLayout = inputLayoutMap["inclass"];
	materialMap["inclass"].pShader = pShaderMap["inclass"];
	materialMap["inclass"].textures.push_back(textureMap["blank"]);
	materialMap["inclass"].samplers.push_back(samplerMap["default"]);

	materialMap["candle"] = materialMap["inclass"];//Copy a material that is already setup.
	materialMap["candle"].textures[0] = textureMap["candle"];

	materialMap["agreatman"] = materialMap["inclass"];//Copy a material that is already setup.
	materialMap["agreatman"].textures[0] = textureMap["agreatman"];

	materialMap["scream"] = materialMap["inclass"];//Copy a material that is already setup.
	materialMap["scream"].textures[0] = textureMap["scream"];

	//Import models
	meshMap["table"] =	new Mesh<VertexPosTexNormTan>();
	meshMap["ghost"] =	new Mesh<VertexPosTexNormTan>();
	meshMap["diamond"] = new Mesh<VertexPosTexNormTan>();
	meshMap["candle"]= new Mesh<VertexPosTexNormTan>();
	ModelImporter::LoadFromFileWithTangents("../Assets/Models/table.obj", (Mesh<VertexPosTexNormTan>*)meshMap["table"], device);
	ModelImporter::LoadFromFileWithTangents("../Assets/Models/ghost.obj", (Mesh<VertexPosTexNormTan>*)meshMap["ghost"], device);
	ModelImporter::LoadFromFileWithTangents("../Assets/Models/diamond.obj", (Mesh<VertexPosTexNormTan>*)meshMap["diamond"], device);
	ModelImporter::LoadFromFileWithTangents("../Assets/Models/candle.obj", (Mesh<VertexPosTexNormTan>*)meshMap["candle"], device);
	
	//Generate Models
	meshMap["ground"] = new Mesh<VertexPosTexNormTan>();
	Geometry::CreateGroundPlane((Mesh<VertexPosTexNormTan>*)meshMap["ground"], device);
	meshMap["frame1"] = new Mesh<VertexPosTexNormTan>();
	Geometry::CreatePictureFrame1((Mesh<VertexPosTexNormTan>*)meshMap["frame1"], device);
	meshMap["frame2"] = new Mesh<VertexPosTexNormTan>();
	Geometry::CreatePictureFrame2((Mesh<VertexPosTexNormTan>*)meshMap["frame2"], device);
	meshMap["wall"] = new Mesh<VertexPosTexNormTan>();
	Geometry::CreateWall((Mesh<VertexPosTexNormTan>*)meshMap["wall"], device);

	//Setup other game settings as needed
	camera.useSpaceCamera = false;
	camera.position = XMFLOAT3(-.6, 1.4f, -1.6f);
	camera.rotation = { .36f, .63f, 0 };
	mouse->InitMouseCallback(gameWindow);//Enable hi-res mouse input
	gameSimpleCBuffer.ambientLightIntensity = ambient;

	//More efficient to allocate ahead of time rather than growing to size slowly.
	opaqueObjects.reserve(15);
	
	GameObject tempGround;
	tempGround.material = &materialMap["inclass"];
	tempGround.mesh = meshMap["ground"];
	tempGround.scale = { 2,2,2 };
	opaqueObjects.push_back(tempGround);

	GameObject tempTable;
	tempTable.material = &materialMap["inclass"];
	tempTable.mesh = meshMap["table"];
	tempTable.scale = XMFLOAT3(.2f, .2f, .2f);
	opaqueObjects.push_back(tempTable);

	GameObject tempFrame;
	tempFrame.material = &materialMap["agreatman"];
	tempFrame.mesh = meshMap["frame1"];

	///What is this number
	//Square root is : 2.2360679775
	//So 3 /2 = 1.5?
	float specialNumber = (1 + sqrt(5)) / 2;
	//https://en.wikipedia.org/wiki/Golden_ratio
	//So loss

	///
	tempFrame.scale = { .2f, .2f * specialNumber, .2f};
	tempFrame.position.x = -.5f;
	tempFrame.position.z = 1;
	tempFrame.position.y = .3f;
	opaqueObjects.push_back(tempFrame);

	tempFrame.material = &materialMap["scream"];
	tempFrame.mesh = meshMap["frame2"];
	tempFrame.position.x = .5f;
	opaqueObjects.push_back(tempFrame);

	GameObject tempWall;
	tempWall.material = &materialMap["inclass"];
	tempWall.mesh = meshMap["wall"];
	tempWall.position.z = 1;
	tempWall.position.y = .25f;
	tempWall.scale.x = 2;
	tempWall.scale.y = .5f;
	opaqueObjects.push_back(tempWall);

	tempWall.rotation.x = XMConvertToRadians(90);
	tempWall.position.z = 0;
	tempWall.position.x = 1;
	opaqueObjects.push_back(tempWall);

	tempWall.rotation.x = XMConvertToRadians(-90);
	tempWall.position.z = 0;
	tempWall.position.x = -1;
	opaqueObjects.push_back(tempWall);

	GameObject tempGhost;
	tempGhost.material = &materialMap["inclass"];
	tempGhost.mesh = meshMap["ghost"];

	tempGhost.rotation.x = 3 * g_XMPi.f[0] / 2;
	tempGhost.position.x = .25f;
	tempGhost.position.z = 0;
	opaqueObjects.push_back(tempGhost);
	
	tempGhost.rotation.x = g_XMPi.f[0];
	tempGhost.position.x = 0;
	tempGhost.position.z = .25f;
	opaqueObjects.push_back(tempGhost);
	
	tempGhost.rotation.x = g_XMPi.f[0] / 2;
	tempGhost.position.x = -.25f;
	tempGhost.position.z = 0;
	opaqueObjects.push_back(tempGhost);

	GameObject tempCandle;
	tempCandle.material = &materialMap["candle"];
	tempCandle.mesh = meshMap["candle"];
	tempCandle.position.y = .1f;
	tempCandle.position.x = .75f;
	opaqueObjects.push_back(tempCandle);

	//Setup convenient references to each object we need direct control of
	//make sure to do this after adding everything to the vector opaqueObjects
	//otherwise the pointers could get invalidated.
	candle = &opaqueObjects.back();
	
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
	//Framerate Independent Gameplay
	float cameraSpeed = 5;
	if (keyboard->ButtonDown(VK_SHIFT))
		cameraSpeed = 15;

	if (keyboard->ButtonDown(VK_ESCAPE))
		PostQuitMessage(0); 

	if(keyboard->ButtonDown('W'))
		camera.MoveForward(cameraSpeed * dt);
	if (keyboard->ButtonDown('S'))
		camera.MoveForward(-cameraSpeed * dt);

	//Panning a camera
	if (keyboard->ButtonDown('D'))
		camera.Strafe(cameraSpeed * dt);
	if (keyboard->ButtonDown('A'))
		camera.Strafe(-cameraSpeed * dt);

	if (!mouse->IsVisible())
	{
		camera.rotation.x += XMConvertToRadians(mouse->DX());
		camera.rotation.y += XMConvertToRadians(mouse->DY());
	}

	if (keyboard->ButtonPressed(VK_TAB))
		if (mouse->IsVisible())
			mouse->HideAndLockMouse(gameWindow);
		else
			mouse->ShowAndUnlockMouse(gameWindow);
} 

//Update is for movement, AI, physics, most gameplay things.
void ExampleGame::Update(float dt)
{
	if (!mouse->IsVisible())
		mouse->CenterCursor();//Do anything that requires mouse position/dx/dy, etc. before this line.
	
	Globals::Get().GetGameCBuffer()->gameTime = timer.TotalGameTime();
	
	camera.Update();

	XMVECTOR candleVec = XMLoadFloat3(&candle->position);
	candleVec = XMVector3Transform(candleVec, XMMatrixRotationY(timer.DeltaTime()));//make candle orbit around table.
	XMStoreFloat3(&candle->position, candleVec);
	candleLight.position = candle->position;
	candleLight.position.y += .2f;
	candleLight.Update();
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
	deviceContext->IASetPrimitiveTopology
	(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	deviceContext->VSSetConstantBuffers(0, 1, &gpuSimpleCBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &gpuSimpleCBuffer); 

	//Draw opaque objects first...
	for (GameObject &g : opaqueObjects)
		g.Draw(); 

	//...Then Draw Transparent Objects...

	//...Then Draw 2D UI/sprites after all 3D game elements

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