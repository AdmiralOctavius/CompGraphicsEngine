	
//Constant Buffer
//Data for rendering objects that is NOT stored in vertices.
//Called a constant buffer because the GPU shaders can't change them
//But we can send over new data from the Game itself (CPU) all the time

//This is GPU side, what our shaders receive from the CPU and draw with
//Notice it must always be in groups of 4 floats.
//If you can't do that add extra 'padding' floats.
cbuffer SimpleCBuffer : register(b0)
{
	//Camera
	float4x4 viewProjection;

	//For objects
	float4x4 objectTransform;
	float4x4 objectInverseTranspose;

	float4 objectColor; 

	//For lighting
	float3 lightPosition;
	float pad;
	float3 lightDirection;
	float lightIntensity;
	float3 lightColor;
	float ambientLightIntensity;
	float falloffStart;
	float falloffEnd;
	float2 pad2;
	float3 cameraPosition;
	float objectSpecularity;

	//Texturing
	float uOffset;
	float vOffset;
	float uScale;
	float vScale;
	//could do float2 texOffset, float2 texScale
	//or float4 texModifier instead

	//2D Sprites
	//Sprites will reuse objectTransform above as well for position/rotation/scale
	float2 screenSize;
	float2 spriteTexPos;//Where in the texture/sprite sheet to pull from - top left corner

	float2 spriteTexSize;//width and height of sprite to pull from texture/spritesheet
	float spriteDepth;
	//extra
	float gameTime;
}

struct VPosTexNormTangentInput
{
	float3 position		: POSITION;
	float2 texCoords    : TEXCOORD0;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPosition		: POSITION;
};

struct VPosTexNormTangentToPixel
{
	float4 position		: SV_POSITION;
	float2 texCoords    : TEXCOORD0;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPosition		: POSITION;
};

typedef VPosTexNormTangentInput StandardVertInput;
typedef VPosTexNormTangentToPixel StandardVertToPixel;