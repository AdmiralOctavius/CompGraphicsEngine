#pragma once

#include "Globals.h"

//point lights emit light in a sphere from a single position
//i.e. Lamps, candles
struct PointLight
{
	XMFLOAT3 position;
	XMFLOAT3 color;
	float intensity;
	float falloffStart;
	float falloffEnd;

	PointLight()
		: position(0, 0, 0),
		color(1, 1, 1),
		intensity(1),
		falloffStart(.5f),
		falloffEnd(1)
	{

	}

	void Update()
	{
		//Set CBuffer Data
		Globals::Get().GetGameCBuffer()->lightPosition = position;
		Globals::Get().GetGameCBuffer()->lightColor = color;
		Globals::Get().GetGameCBuffer()->lightIntensity = intensity;
		Globals::Get().GetGameCBuffer()->falloffStart = falloffStart;
		Globals::Get().GetGameCBuffer()->falloffEnd = falloffEnd;

		//Send CBuffer Data (from GameCBuffer to GPUCBuffer)
		Globals::Get().GetDeviceContext()->UpdateSubresource(
			Globals::Get().GetGPUCBuffer(),
			0, NULL,
			Globals::Get().GetGameCBuffer(),
			0, 0);
	}
};

//Emits light in one direction from a start point
//i.e. a flashlight
struct SpotLight
{
	XMFLOAT3 position;
	XMFLOAT3 direction;
	XMFLOAT3 color;
	float intensity;

	SpotLight()
		: position(0, 0, 0),
		direction(0, -1, 0),
		color(1, 1, 1),
		intensity(1)
	{

	}

	void Update()
	{
		//Set CBuffer Data
		XMStoreFloat3(&direction, XMVector3Normalize(XMLoadFloat3(&direction)));
		Globals::Get().GetGameCBuffer()->lightPosition = position;
		Globals::Get().GetGameCBuffer()->lightDirection = direction;
		Globals::Get().GetGameCBuffer()->lightColor = color;
		Globals::Get().GetGameCBuffer()->lightIntensity = intensity;

		//Send CBuffer Data (from GameCBuffer to GPUCBuffer)
		Globals::Get().GetDeviceContext()->UpdateSubresource(
			Globals::Get().GetGPUCBuffer(),
			0, NULL,
			Globals::Get().GetGameCBuffer(),
			0, 0);
	}
};

//directional lights are treated as being infinately far away
//with all rays of light being parallel in one direction
//i.e. the sun
struct DirectionalLight
{
	DirectionalLight()
		: direction(1, 0, 0),
		color(1, 1, 1),
		intensity(1)
	{

	}

	void Update()
	{
		//Set CBuffer Data
		XMStoreFloat3(&direction, XMVector3Normalize(XMLoadFloat3(&direction)));
		Globals::Get().GetGameCBuffer()->lightDirection = direction;
		Globals::Get().GetGameCBuffer()->lightColor = color;
		Globals::Get().GetGameCBuffer()->lightIntensity = intensity;

		//Send CBuffer Data (from GameCBuffer to GPUCBuffer)
		Globals::Get().GetDeviceContext()->UpdateSubresource(
			Globals::Get().GetGPUCBuffer(),
			0, NULL,
			Globals::Get().GetGameCBuffer(),
			0, 0);
	}

	XMFLOAT3 direction;
	XMFLOAT3 color;
	float intensity;
};