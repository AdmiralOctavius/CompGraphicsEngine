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
	RenderManager::CreateRasterizerState(rasterizerState, device, D3D11_FILL_SOLID, D3D11_CULL_BACK);//D3D11_CULL_NONE = setting twosided material in Unreal
	RenderManager::CreateRasterizerState(wireFrameState, device, D3D11_FILL_WIREFRAME, D3D11_CULL_BACK);
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
	RenderManager::CreateVertexShaderAndInputLayout<VertexPositionColor>(L"VS_color.cso", "color", vShaderMap, inputLayoutMap, device);
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

	materialMap["color"] = Material();
	materialMap["color"].rasterizerState = rasterizerState;
	materialMap["color"].depthState = standardDepthState;
	materialMap["color"].blendState = noBlend;
	materialMap["color"].vShader = vShaderMap["color"];
	materialMap["color"].inputLayout = inputLayoutMap["color"];
	materialMap["color"].pShader = pShaderMap["color"];

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

	
	Geometry::CreateTriangle(&triangleMesh, device);
	triangle.mesh = &triangleMesh;
	triangle.material = &materialMap["color"];
	triangle.position = XMFLOAT3(0, 0, -10);

	Geometry::CreateSquare(&squareMesh, device);
	square.mesh = &squareMesh;
	square.material = &materialMap["color"];
	square.position = XMFLOAT3(-5, 0, -10);

	Geometry::CreateBetterSquare(&betterSquareMesh, device);
	betterSquare.mesh = &betterSquareMesh;
	betterSquare.material = &materialMap["color"];
	betterSquare.position = XMFLOAT3(-7, 0, -10);

	//Import models
	meshMap["Table"] = new Mesh<VertexPositionColor>();
	meshMap["Skybox"] = new Mesh<VertexSky>();
	meshMap["Kirby"] = new Mesh<VertexPosNormTexture>();
	meshMap["Barrel"] = new Mesh<VertexPosNormTexture>();
	meshMap["Ground"] = new Mesh<VertexPosNormTexture>();
	meshMap["Well"] = new Mesh<VertexPosTexNormTan>();

	//Added Game object
	meshMap["Computer"] = new Mesh<VertexPositionColor>();
	meshMap["Katana"] = new Mesh<VertexPositionColor>();
	meshMap["Bike"] = new Mesh<VertexPositionColor>();

	ModelImporter::LoadFromFile("../Assets/Models/table.obj", (Mesh<VertexPositionColor>*)meshMap["Table"], device);
	ModelImporter::LoadFromFile("../Assets/Models/invertedcube.obj", (Mesh<VertexSky>*)meshMap["Skybox"], device);
	ModelImporter::LoadFromFile("../Assets/Models/kirby.obj", (Mesh<VertexPosNormTexture>*)meshMap["Kirby"], device);
	ModelImporter::LoadFromFile("../Assets/Models/barrel.obj", (Mesh<VertexPosNormTexture>*)meshMap["Barrel"], device);
	ModelImporter::LoadFromFile("../Assets/Models/ground.obj", (Mesh<VertexPosNormTexture>*)meshMap["Ground"], device);
	ModelImporter::LoadFromFileWithTangents("../Assets/Models/well.obj", (Mesh<VertexPosTexNormTan>*)meshMap["Well"], device);
	//Added Game Object
	ModelImporter::LoadFromFile("../Assets/Models/Comp_and_Floppy.obj", (Mesh<VertexPositionColor>*)meshMap["Computer"], device);
	ModelImporter::LoadFromFile("../Assets/Models/fantasy_katana.obj", (Mesh<VertexPositionColor>*)meshMap["Katana"], device);	
	ModelImporter::LoadFromFile("../Assets/Models/BSA_BantamD1_OBJ.obj", (Mesh<VertexPositionColor>*)meshMap["Bike"], device);	


	//Setup other game settings as needed
	camera.useSpaceCamera = false;
	camera.position = XMFLOAT3(0, 2, -15);
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

	opaqueObjects.push_back(GameObject());
	opaqueObjects.back().material = &materialMap["color"];
	opaqueObjects.back().mesh = meshMap["Table"];
	opaqueObjects.back().position.z += 5;
	//opaqueObjects.back().position.y += 5;
	a3ControlledObject = &opaqueObjects.back();
	
	GameObject kirby;
	kirby.material = &materialMap["kirby"];
	kirby.mesh = meshMap["Kirby"];
	kirby.rotation.x = XMConvertToRadians(90);
	kirby.position.x += 5;
	opaqueObjects.emplace_back(kirby);

	
	playerKirby.material = &materialMap["kirby"];
	playerKirby.mesh = meshMap["Kirby"];
	playerKirby.position.x += 20.5;
	playerKirby.position.y += 5;
	playerKirby.rotation.x = XMConvertToRadians(90);
	playerKirby.SetColor(XMFLOAT4(0.5, 0.5, 1, 1));
	playerKirby.playerInput = true;
	actorObjects.emplace_back(playerKirby);

	enemyKirby.material = &materialMap["kirby"];
	enemyKirby.mesh = meshMap["Kirby"];
	enemyKirby.position.x += -20.5;
	enemyKirby.position.y += 1;
	enemyKirby.SetColor(XMFLOAT4(0.5, 0, 1, 1));
	actorObjects.emplace_back(enemyKirby);

	addedDrawnObjects.push_back(GameObject());
	addedDrawnObjects.back().material = &materialMap["color"];
	addedDrawnObjects.back().mesh = meshMap["Computer"];
	addedDrawnObjects.back().position.z += 2;
	addedDrawnObjects.back().position.y += 0.89;
	addedDrawnObjects.back().SetColor(XMFLOAT4(0.75, 0.75, 0.75, 1));
	addedDrawnObjects.back().scale = XMFLOAT3(0.15, 0.15, 0.15);

	opaqueObjects.push_back(GameObject());
	opaqueObjects.back().material = &materialMap["color"];
	opaqueObjects.back().mesh = meshMap["Table"];
	opaqueObjects.back().position.z += 2;
	opaqueObjects.back().SetColor(XMFLOAT4(0.36, 0.25, 0.20, 1));
	//opaqueObjects.back().position.y += 5;
	
	addedDrawnObjects.push_back(GameObject());
	addedDrawnObjects.back().material = &materialMap["color"];
	addedDrawnObjects.back().mesh = meshMap["Katana"];
	addedDrawnObjects.back().position.z += 1.8;
	addedDrawnObjects.back().position.x += 0.35;
	addedDrawnObjects.back().position.y += 0.8;
	addedDrawnObjects.back().rotation.z += XMConvertToRadians(100);
	addedDrawnObjects.back().SetColor(XMFLOAT4(0, 0.75, 0.75, 1));
	addedDrawnObjects.back().scale = XMFLOAT3(0.025, 0.025, 0.025);

	addedDrawnObjects.push_back(GameObject());
	addedDrawnObjects.back().material = &materialMap["color"];
	addedDrawnObjects.back().mesh = meshMap["Bike"];
	addedDrawnObjects.back().position.z += 1.5;
	addedDrawnObjects.back().position.x += -2;
	addedDrawnObjects.back().position.y += 0.65;
	//addedDrawnObjects.back().rotation.z += 100;
	addedDrawnObjects.back().SetColor(XMFLOAT4(1, 1, 1, 1));
	addedDrawnObjects.back().scale = XMFLOAT3(0.0025, 0.0025, 0.0025);


	for (int i = 1; i < 5; ++i)
	{
		kirby.position.x += 2;
		//kirby.scale.x = kirby.scale.y = kirby.scale.z = i;//uniform scale
		//kirby.scale.x = i;//non-uniform scale
		kirby.SetColor(Utility::RandomColor());
		opaqueObjects.emplace_back(kirby);
	}

	for (int i = 0; i < 5; ++i)
		kirbyArmy.emplace_back(&opaqueObjects[i + 3]);

	/*Color
	Subtractive - All colors together = black - Paint (real life color)
	
	Additive - Digital Colors - All colors together = White
		White = (1, 1, 1, 1), Black = (0, 0, 0, 1);
		Color = RGBA = Red, Green, Blue, Alpha
		Blue = (0, 0, 1, 1);
		Light Blue = (.5, .5, 1, 1);
		(0, 1, 1, 1) = Cyan

	Coordinate Spaces
		Origin, Axis
		World Space = X, Y, Z, and Global (0, 0, 0) is the origin
		Object Space = X, Y, Z, and the origin is the object's position

	Color spaces
		Are what range of colors we can create
		RGB - normal range 0 is black, 1 is white, (1, 0, 0) is red, etc.
		sRGB - standard RGB - standardized across all devices
		CMYK - Printers (subtractive) 
			- Cyan, Magenta, Yellow, Black

		Blue * Red
		(0, 0, 1, 1) 
	  * (1, 0, 0, 1)
	  _______________
	    (0, 0, 0, 1) = Black

		Blue + Red
		(0, 0, 1, 1) 
	  * (1, 0, 0, 1)
	  _______________
	    (1, 0, 1, 1) = Purple

	Red = (1, 0, 0, 1) (as floats) OR (255, 0, 0, 255) (as ints)
	(128, 128, 0, 255) = Yellow

	Channels are the individual components of a color
	Red Channel, Blue Channel, Green Channel, Alpha Channel

	Hue	= What color is it? Where on the color wheel?
	Saturation = How much color? 0 = grayscale, 1 = vibrant color
	Value (Luminosity) = Brightness, 1 = white, 0 = black

	Direct X Math
	DirectX takes advantage of a hardware technique called
	SIMD = Single Instruction, Multiple Data
	Normally code is executed 1 thread at a time.
	Context Switching = How a cpu switches from one 
		process to another over and over very quickly,
		in order to give the appearance that multiple
		programs are running at once.

		(0, 0, 1, 1) 
	  * (1, 0, 0, 1)
	  Sequentially, it would do 0 * 1, then 0 * 0, then 1 * 0, then 1 * 1, then return result

	  SIMD allows the computer to process multiple pieces of data simultaneously
	  So instead of 1 by 1, it does
	  0 * 1
	  0 * 0
	  1 * 0
	  1 * 1
	  At the same time instead of 1 after the other.

	  This is why DirectXMath looks the way it does and is annoying to use
		so that its up to 4x faster than not using it

	When you store data persistently, use XMFLOATX
	When you do operations on data, use XMVECTOR or XMMATRIX
	To convert use XMLoadFloatX and XMStoreFloat

	Scalar = 1 value (what you are used to)
		Ex. 2, x, speed.x, gameTime, deltaTime
	Scalar Operations: +, -, /, *, %, 
		Round, Ceil, Floor,
		Round(4.6)	= 5 
		Ceil(4.6)	= 5 = Round but always up
		Floor(4.6)	= 4 = Truncate(4.6) = Round but always down

		Distributive
			a(b + c) = ab + ac

		Commutative
			a + b = b + a

	Vector (more than 1 value in a row)
		Vector2 or Vector3 or Vector4
		Origin (0, 0, 0)
		Position (5, 0, 5)
		Velocity (2, 2, 0)
		Acceleration (0, -9.81, 0)

		(5, 0, 0) + (-5, 0, 0)	= (x1 + x2, y1 + y2, z1 + z2)
								= (0, 0, 0)

		(2, 2, 0) * 5 =	(x1 * 5, y1 * 5, z1 * 5) = (10, 10, 0)

		Magnitude of a vector
		(2, 2, 0) = (use pythagorean theorem)
		a^2 + b^2 + z^2 = c^2
		
		Distance between 2D Points:
		(2, 1), (4, 5)
		= sqrt((y2 - y1)^2 + (x2 - x1)^2)
		= find the hypoteneuse of the right triangle between the two points

		Distance between 3D Points:
		(2, 1, 5), (4, 5, 8)
		= sqrt((y2 - y1)^2 + (x2 - x1)^2 + (z2 - z1)^2)

		Normalizing = Returns a VECTOR that is the same direction
			as the input vector, but has a magnitude of 1.
			aka gives the direction a vector, without the length
		normalize(0, -9.81, 0) =  (0, -1, 0)
		normalize(0, -934.81, 0) =  (0, -1, 0)
		normalize(0, -2983.81, 0) =  (0, -1, 0)
		normalize(2, 2, 0) = (.707, .707, 0)
				(its magnitude or hypoteneuse length is 1)

		Cross Product (returns vector)
		 a x b = results in a vector perpendicular to a & b
		 i.e. Cross(forward, up) = right vector

		Dot Product (returns scalar)
		 a • b = ||a||*||b||*cos(Θ)
		 If a & b are normalized = 1 * 1 * cos(Θ) = cos(Θ)
		 So the angle Θ between two vectors is just Acos(a•b)
		 
		 ||a|| = magnitude (or length) of a vector
		 |a| = absolute value of a scalar a
		 
		 = a ratio of the angle between two vectors
		 if a • b = 1,	then a & b are parallel, and face the same direction
		 if a • b = -1, then a & b are parallel, and face opposite directions
		 if a • b = 0,	then a & b are perpendicular
		 
		 Used for 
		 Lighting (how much is a light aimed at a surface to light it up)
		 Determine if 1 character is facing towards another character or facing away

	Matrix
		Rows and Columns (table) of numbers
		Row Column order

		Identity Matrix
		[1, 0, 0, 0] 4x4 matrix (rows x columns)
		[0, 1, 0, 0]
		[0, 0, 1, 0]
		[0, 0, 0, 1]

		Matrix A
		[1, 2]
		[3, 4]

		Transpose(A) 
		Transposing a matrix means switching its rows and columns
		[1, 3]
		[2, 4]

		   A        B
		[1, 2]   [4,  6]   [A11*B11+A12*B21  B]   [1*4+2*-3, 1*6+2*2]   
		[3, 4] * [-3, 2] = [C				 D] = [3*4+4*-3, 3*6+4*2]

		[1*4+2*-3, 1*6+2*2]	  [-2, 10]
		[3*4+4*-3, 3*6+4*2] = [ 0, 26]

		Matrix math is NOT COMMUTATIVE

		[1, 0, 0|3x3 matrix
		[0, 1, 0|
		[0, 0, 1|

		[1, 2, 3] 1x3 matrix (like a vector)

		[1, 2, 3] 2x3 matrix
		[4, 5, 6]

		I * A = A
		I * B = B
		Like the number 1 but for matrices
		So it represents a transform that has no rotation/scale/translation

	*/

	XMFLOAT2 position2D = { 400, 300 };
	XMFLOAT3 position = { 0, 0, 0 };//similar to Vector3 in Unity/Unreal
	XMFLOAT4 color = { 1, 0, 1, 1 };//Purple

	XMFLOAT3 velocity = { 5, 5, 0 };
	XMVECTOR velocityVec = XMLoadFloat3(&velocity);
	XMVECTOR normalizedVelocity = XMVector3Normalize(velocityVec);
	XMStoreFloat3(&velocity, normalizedVelocity);
	cout << Utility::VectorToString(velocityVec) << endl;
	cout << Utility::VectorToString(normalizedVelocity) << endl;

	XMVECTOR posVector = XMVectorSet(0, 0, 0, 1);
	XMMATRIX transformMatrix = XMMatrixIdentity();
	cout << endl << Utility::MatrixToString(transformMatrix) << endl;

	transformMatrix = XMMatrixTranslation(5, 10, 0);
	cout << endl << Utility::MatrixToString(transformMatrix) << endl;

	//Scaling = make things bigger (or smaller)
	//Uniform Scale - scaling the same amount on all 3 axiis
	//non-uniform scaling - scaling 1 axis differently from another
	transformMatrix = XMMatrixScaling(2, 2, 2);
	cout << endl << Utility::MatrixToString(transformMatrix) << endl;

	transformMatrix = XMMatrixRotationY(45);
	transformMatrix = XMMatrixRotationY(XMConvertToRadians(45));
	cout << endl << Utility::MatrixToString(transformMatrix) << endl;

	//this one applies any matrix to a single vector.
	//XMVector3Transform()

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
	//dt = seconds/frame
	//speed = units/second
	//units/frame

	//Framerate Independent Gameplay
	float kirbySpeed = 5;//per second
	float cameraSpeed = 5;
	if (keyboard->ButtonDown(VK_SHIFT))
		cameraSpeed = 15;

	if (keyboard->ButtonDown(VK_ESCAPE))
		PostQuitMessage(0); 

	/*if (keyboard->ButtonDown(VK_UP))
	{
		for(int i = 0; i < kirbyArmy.size(); ++i)
			kirbyArmy[i]->position.z += kirbySpeed * dt;
	}

	if (keyboard->ButtonDown(VK_DOWN))
		for (int i = 0; i < kirbyArmy.size(); ++i)
			kirbyArmy[i]->position.z -= kirbySpeed * dt;
*/


	if(keyboard->ButtonDown('W'))
		camera.MoveForward(cameraSpeed * dt);
	if (keyboard->ButtonDown('S'))
		camera.MoveForward(-cameraSpeed * dt);

	//Panning a camera
	if (keyboard->ButtonDown('D'))
		camera.Strafe(cameraSpeed * dt);
	if (keyboard->ButtonDown('A'))
		camera.Strafe(-cameraSpeed * dt);

	//Degrees Vs. Radians 
	//ALT+227 1π * 180/π
	//Degrees
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

	//Direction distance testing
	if (keyboard->ButtonPressed('P')) {
		cout << opaqueObjects.front().GetDistance(XMFLOAT3(0, 0, 0)) << "\n";
		cout << opaqueObjects[3].GetDistance(XMFLOAT3(0, 0, 0)) << "\n";
		cout << opaqueObjects.back().GetDistance(XMFLOAT3(0, 0, 0)) << "\n";
	}

	if (keyboard->ButtonDown(VK_RETURN)) {
		moveEnemy = true;
		
	}
	else {
		moveEnemy = false;
	}


	//PlayerKirby input
	if (playerKirby.playerInput == true) {
		//cout << "Got here in player input check";
		//Right
		if (keyboard->ButtonDown(VK_RIGHT)) {
			//actorObjects[0].position.x += 20 * dt;
			actorObjects[0].MoveForward(-4 * dt);
		}
		//Backward
		if (keyboard->ButtonDown(VK_DOWN)) {
			//actorObjects[0].position.z -= 20 * dt;
			actorObjects[0].Strafe(-4 * dt);
			
		}
		//Left
		if (keyboard->ButtonDown(VK_LEFT)) {
			//actorObjects[0].position.x -= 20 * dt;
			
			actorObjects[0].MoveForward(4 * dt);
			//cout << "Got here, new pos is: " << actorObjects[0].position.x;
		}
		if (keyboard->ButtonDown(VK_UP)) {
			//actorObjects[0].position.z += 20 * dt;
			
			actorObjects[0].Strafe(4 * dt);
		}
		if (keyboard->ButtonDown('U')) {
			actorObjects[0].rotation.x -= XMConvertToRadians( 360 * dt);
			//cout << "New X Rot: " << actorObjects[0].rotation.x << "\n";
		}
		if (keyboard->ButtonDown('O')) {
			actorObjects[0].rotation.x += XMConvertToRadians(360 * dt);
			//cout << "New X Rot: " << actorObjects[0].rotation.x << "\n";
		}
		if (keyboard->ButtonDown('I')) {
			actorObjects[0].position.y += 2 * dt;
			
		}
		if (keyboard->ButtonDown('K')) {
			actorObjects[0].position.y -= 2 * dt;

			
		}




	}
} 

//Update is for movement, AI, physics, most gameplay things.
void ExampleGame::Update(float dt)
{
	if (!mouse->IsVisible())
		mouse->CenterCursor();//Do anything that requires mouse position/dx/dy, etc. before this line.
	
	XMFLOAT4 lerpedColor;
	XMStoreFloat4(&lerpedColor, XMVectorLerp({ 1,0,0,1 }, { 0, 0, 1, 1 }, sin(timer.TotalGameTime())));
	opaqueObjects[3].SetColor(lerpedColor);
	kirbyArmy[1]->rotation.x += 10* timer.DeltaTime();
	Globals::Get().GetGameCBuffer()->gameTime = timer.TotalGameTime();
	sun.Update();
	camera.Update();
	sky.position = camera.position;
	actorObjects[0].Update(dt);

	actorObjects[1].MoveTo(actorObjects[0].position, 0.11*dt);
	if (moveEnemy == true) {

	}
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
	//every 3 vertices = 1 triangle.
	//D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	//The first 3 vertices form a triangle, 
		//And from then on form a triangle between the next vertex
		//and the previous 2
	//1   2  5  7
	//3   4  6  8
	
	//Triangle Fan


	
	deviceContext->VSSetConstantBuffers(0, 1, &gpuSimpleCBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &gpuSimpleCBuffer); 

	//Draw opaque objects first
	for (GameObject &g : opaqueObjects)
		g.Draw(); 

	for (GameObject &a : actorObjects) {
		a.Draw();
	}

	for (GameObject &o : addedDrawnObjects) {
		o.Draw();
	}

	triangle.Draw();
	betterSquare.Draw();

	//square.SetColor(Utility::RandomColor());
	square.Draw();

	//Then draw background/skybox
	sky.Draw();
		
	//Draw 2D UI/sprites after all 3D game elements
	//Draw text to the screen
	spriteBatch->Begin();
	{
		spriteFont->DrawString(spriteBatch, 
			std::wstring(L"FPS: " + std::to_wstring((int)frameRate)).c_str(), XMFLOAT2(0, 0), XMVectorSet(1, 0, 0, 1));
	}
	spriteBatch->End();//Text doesn't actually get drawn to the screen until here

	//Finally present new image to the screen
	HR(swapChain->Present(vsync, 0));

	//VSync = Vertical Sync
	//Solves the problem of Screen Tearing
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