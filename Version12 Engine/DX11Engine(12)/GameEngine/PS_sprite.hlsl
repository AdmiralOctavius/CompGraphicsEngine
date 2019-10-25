#include "Common.hlsli"

Texture2D spriteTexture : register(t0);
SamplerState standardSampler : register(s0);

float4 main(VSpriteToPixel input) : SV_TARGET
{
	float3 color = objectColor.xyz * spriteTexture.Sample(standardSampler, input.texCoords).xyz;

	return float4(color, 1);
}