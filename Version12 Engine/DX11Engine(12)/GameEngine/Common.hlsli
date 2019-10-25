	
//Constant Buffer
//Data for rendering objects that is NOT stored in vertices.
//Called a constant buffer because the GPU shaders can't change them
//But we can send over new data from the Game itself (CPU) all the time

//This is GPU side, what our shaders receive from the CPU and draw with
cbuffer SimpleCBuffer : register(b0)
	{
		float4x4 viewProjection;

		//For objects
		float4x4 objectTransform;
		float4x4 objectInverseTranspose;
		float4 objectColor; 

		//For lights
		float3 lightDirection;
		float lightIntensity;
		float3 lightColor;
		float ambientLightIntensity;

		float3 cameraPosition;
		float objectSpecularity;

		float uOffset;
		float vOffset;
		float uScale;
		float vScale;
		//could do float2 texOffset, float2 texScale
		//or float4 texModifier instead

		//Sprites will reuse objectTransform above as well for position/rotation/scale
		float2 screenSize;
		float2 spriteTexPos;//Where in the texture/sprite sheet to pull from - top left corner
		float2 spriteTexSize;//width and height of sprite to pull from texture/spritesheet
		float spriteDepth;

		float gameTime;
	}

struct VPosColInput//Where vertex data gets stored on the GPU when we call deviceContext->VSSetVertexBuffer in ExampleGame::Draw
{
	float3 position : POSITION;
	float4 color	: COLOR;
};

struct VPosColToPixel//What the vertex shader outputs, and the pixel shader will receive
{
	float4 position	: SV_POSITION;//System-Value Position - tells the GPU this is the 2D screen position of the vertex and use it to determine which pixels to process
	float4 color	: COLOR;
};

//Surface Normals are vectors that indicate the direction a face is facing
struct VPosColNormInput
{
	float3 position : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
};

struct VPosColNormToPixel
{
	float4 position : SV_POSITION;//position is in screen space based on camera
	float3 worldPosition : POSITION;//3D world space position (including gameobject's position, rotation, scale)
	float4 color : COLOR;
	float3 normal : NORMAL;
};

//Surface Normals are vectors that indicate the direction a face is facing
struct VPosNormTextureInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoords : TEXCOORD0;
};

struct VPosNormTextureToPixel
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION;
	float2 texCoords : TEXCOORD0;
	float3 normal : NORMAL;
};

struct VSkyInput
{
	float3 position : POSITION;
};

struct VSkyToPixel
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION;
};

struct VPosTexNormTangentInput
{
	float3 position		: POSITION;//Mesh Space
	float2 texCoords    : TEXCOORD0;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPosition		: POSITION;//World Space
};

struct VPosTexNormTangentToPixel
{
	float4 position		: SV_POSITION;//Screen Space
	float2 texCoords    : TEXCOORD0;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPosition		: POSITION;
};

typedef VPosTexNormTangentInput StandardVertInput;
typedef VPosTexNormTangentToPixel StandardVertToPixel;

struct VSpriteInput
{
	float3 position		: POSITION;
	uint2 index			: INDEX;
};

struct VSpriteToPixel
{
	float4 position		: SV_POSITION;
	float2 texCoords	: TEXCOORD0;
};