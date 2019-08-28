#pragma once

struct SimpleConstantBuffer
{
	//Camera
	XMFLOAT4X4 viewProjection;
	
	//For objects
	XMFLOAT4X4 objectTransform;
	XMFLOAT4X4 inverseTranspose;
	XMFLOAT4 objectColor;

	//For a directional light
	XMFLOAT3 lightDirection;
	float lightIntensity;
	XMFLOAT3 lightColor;
	float ambientLightIntensity;

	XMFLOAT3 cameraPosition;
	float objectSpecularity;

	float uOffset = 0;
	float vOffset = 0;
	float uScale = 1;
	float vScale = 1;

	XMFLOAT2 screenSize;
	XMFLOAT2 spriteTexPos;
	XMFLOAT2 spriteTexSize;
	float spriteDepth;
};