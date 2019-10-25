#include "Common.hlsli"

StandardVertToPixel main(StandardVertInput input)
{
	StandardVertToPixel output;

	output.worldPosition 
		= mul(float4(input.position, 1), objectTransform).xyz;
	//float4.xyz or float4.xz or float4.rggb
	//This is called swizzle

	output.position = mul(float4(input.position, 1), 
		mul(objectTransform, viewProjection));
	
	output.normal = input.normal;//pass-through (shader) (not processing here)

	return output;
}