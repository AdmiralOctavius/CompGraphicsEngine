#include "Common.hlsli"

StandardVertToPixel main(StandardVertInput input)
{
	StandardVertToPixel output;

	output.worldPosition = mul(float4(input.position, 1), objectTransform).xyz;//.xyz called a swizzle
	output.position = mul(float4(input.position, 1), mul(objectTransform, viewProjection));
	output.texCoords = input.texCoords * float2(uScale, vScale) + float2(uOffset, vOffset);
	
	//The 2nd one fixes issues with lighting objects that have non-uniform scale:
	//output.normal = mul(input.normal, (float3x3)objectTransform);
	output.normal = mul(input.normal, (float3x3)objectInverseTranspose);

	return output;
}