#include "Common.hlsli"

Texture2D diffuseTexture : register(t0);
SamplerState standardSampler : register(s0); 

float4 main(StandardVertToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float3 ambient;
	ambient.rgb = ambientLightIntensity;

	float4 textureColor = diffuseTexture.Sample(standardSampler, input.texCoords);

	//Point Light Pseudocode for assignment 4.
	//Refer to the variables in Common.hlsli's to see what you have available
	//Also refer to the lighting chapter in the book, but remember his code is different from ours in structure and names.
		
		//First, we need to calculate the direction from the light to the pixel we are working on.
		//Get that vector (a float3) by subtracting the light's position from the pixel's world position

		//Then store the length of that vector in a variable using the HLSL function length(vector)

		//if the length of the vector is larger than the outer falloff radius of the light (falloffEnd)
			//then stop early and just return the ambient light color only.

		//Normalize the light vector you calculated earlier.
		
		//Calculate the ndotl lighting of this pixel as the dot product of the light vector and the input normal
			//Notice that if you subtract the vectors in the order above you do NOT need to negate anything here
			//like we did with directional lights.
		//multiply the result by the intensity of the light
		//saturate the result (call the saturate HLSL function on it) to clamp it to a 0-1 range.

		//This would be a good place to return float4(ndotl, ndotl, ndotl, 1); to see if its working so far.

		//Next we want the light to 'falloff' or die off over distance
		//calculate a scalar value called attenuation as (falloffEnd - length of the light vector) / (falloffEnd - falloffStart)
		//saturate the result.

		//finally, calulate the final diffuse lighting factor by multiplying the color of the light by the ndotl and attenuation values.

		//Now you can return this value to see just the diffuse lighting of the scene. But we aren't done!

		//Next we need to calculate the Specular factor of the light (highlights)
			//This is just like we did with Directional Lights in class
		//Make a float3 to store our final specularity value and initialize it to 0
		//if the ndotl we calculated earlier is greater than 0,
			//calculate the eyeDirection vector by subtracting the camera position from the pixel's world position, and normalize the result
			//calculate the reflection of the light using the HLSL function reflect(the inverse (negative) light direction, the input normal);
			//calculate the dot product of the reflection vector and eyeDirection vectors
			//saturate the result
			//raise the result to the power in the variable objectSpecularity to soften the specular highlights.
			//multiply the result by the attenuation you calculated earlier.
			//and finally store the result in the specularity variable you set to 0 earlier.
		
		//Now, Finally, put it all together.
		//the final color of the pixel should be the textureColor * the diffuse lighting + the specular lighting + the ambient lighting.
		//remember to append a 1 for alpha, like return float4(finalColor, 1);

	//Use these to see what's going on quickly. Add more as needed.

	//See pure texture color only
	//return textureColor;
	
	//see ambient light only
	//return float4(ambient, 1);

	//This converts normals from -1-1 to 0-1, so instead of seeing black
	//half the time you see different colors for all normals.
	float3 normalsIn0to1Range = (input.normal + 1) / 2;
	return float4(normalsIn0to1Range, 1.0f);

	//This lets you visualize the normals
	//return float4(input.normal, 1.0f);
}