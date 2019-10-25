/*HLSL Notes
	HLSL Functions are called 'Intrinsics'
		length(vector), saturate(value), reflection(), dot(),
		normalize(), pow(), and many more.

	Phong Material (Phong Shading)
	Diffuse lighting + Specular Lighting + Ambient Lighting

	Specularity or Specular Lighting refers to
	the highlights or reflections of light sources in objects

	Saturate(float) clamps a value to a 0-1 range
	2^2 = 4
	10^2 = 100
	.5 ^2 = .25
	Pow on values less than 1 decreases the value

	color.rgb
	color.xyz
	color.zyx
	color.bag
	color.rgba
	are all different ways to 'swizzle' the values of a float2-4

	[flatten] 
	on an if statement, evaluate and just discard if false. 
	Can be more efficient in GPU's than actually deciding to run or not.

	[unroll]
	on a for loop (we don't have one here) evaluate as if each iteration is copy-pasted code instead of a single line that loops.
	Can be more efficient in GPU's than conventional looping.
*/

#include "Common.hlsli"

float4 main(StandardVertToPixel input) : SV_TARGET
{
	//Fix normals that are no longer normalized after the rasterizer blends them across each pixel.
	input.normal = normalize(input.normal);
	
	//Directional Light Diffuse Lighting
	float ndotl = saturate(dot(input.normal, -lightDirection));//how much light to apply to this pixel
	float3 diffuse = objectColor * lightColor * ndotl;

	//Directional Light Specularity Lighting
	float3 specularity = 0;
	[flatten]
	if(ndotl > 0)//without this we get specular highlights even on the dark side of the mesh
	{
		float3 eyeDir = normalize(cameraPosition - input.worldPosition);
		float3 reflection = reflect(lightDirection, input.normal); 
		specularity = pow(saturate(dot(reflection, eyeDir)), objectSpecularity); 
	}  

	//Put it all together and return the result.
	return float4(diffuse + specularity + ambientLightIntensity, 1); 
}