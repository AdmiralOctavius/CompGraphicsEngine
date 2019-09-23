#include "Common.hlsli"

//Vertex Shader
//The main purpose of a vertex shader is to project our 3D world onto our 2D screen
VPosColToPixel main(VPosColInput input)
{
	VPosColToPixel output;

	//Part four of assignment
	/* Commented out for sanity's sake
	input.position.xy += .5f * sin(input.position.x) * sin(3.0f * gameTime);
	input.position.z *= .6f + .4f * sin(2.0f * gameTime);
	*/
	//Transform to Screen Space
	//Take 3D position, put it where the object is, then project it into 2D
	output.position = mul(float4(input.position, 1), 
					mul(objectTransform, viewProjection));

	output.color = input.color * objectColor;

	return output;
}