#pragma once

#include <DirectXMath.h>
#include <cmath>
#include "ConstantBufferDefinitions.h"
#include "Mesh.h" 
#include "Globals.h"
#include "Material.h"

using namespace std;

class GameObject
{
	XMFLOAT4 color; //use SetColor function instead so it can be premultiplied by alpha if desired
	BoundingBox boundingBox;
	BoundingSphere boundingSphere;

public:
	BaseMesh* mesh = nullptr;
	Material* material = nullptr;
	
	XMFLOAT4X4 transform;//Transformation Matrix - stores position, rot, scale. Also usually called this object's 'world' matrix
	XMFLOAT3 position, forward, up, right;
	//rotation.x = Yaw, rotation.y = Pitch, rotation.z = Roll.
	XMFLOAT3 rotation;
	XMFLOAT3 scale, velocity, acceleration;
	XMFLOAT2 uvScale;
	float specIntensity, depthFromCamera;
	float totalPitch, maxPitch;

	//Imported from Camera


	bool playerInput = false;

	GameObject()
		: position(0, 0, 0),
		rotation(0, 0, 0),
		forward(0, 0, 1),
		up(0, 1, 0),
		uvScale(1, 1),
		scale(1, 1, 1),
		velocity(0, 0, 0),
		acceleration(0, 0, 0),
		color(1, 1, 1, 1),
		specIntensity(64)
	{

	}

	void SetColor(XMFLOAT4 col, bool premultiplyAlpha = false)
	{
		color = col;
		if (premultiplyAlpha)
		{
			color.x *= color.w;
			color.y *= color.w;
			color.z *= color.w;
		}
	}

	void Update(float dt)
	{
		//AI Movement, physics (velocity/gravity), color changes, etc. could go here.
		XMVECTOR a = XMLoadFloat3(&acceleration);
		XMVECTOR v = XMLoadFloat3(&velocity);
		XMVECTOR p = XMLoadFloat3(&position);
		v += a * dt;
		p += v * dt;
		XMStoreFloat3(&velocity, v);
		XMStoreFloat3(&position, p);
		//ContinuousUpdate();
		RecalculatedUpdate();
	}

	BoundingBox GetBoundingBox()
	{
		boundingBox = mesh->boundingBox;
		boundingBox.Center = Utility::Add(boundingBox.Center, position);
		boundingBox.Extents.x *= scale.x;
		boundingBox.Extents.y *= scale.y;
		boundingBox.Extents.z *= scale.z;
		return boundingBox;
	}

	BoundingSphere GetBoundingSphere()
	{
		boundingSphere = mesh->boundingSphere;
		boundingSphere.Center = Utility::Add(boundingBox.Center, position);
		boundingSphere.Radius *= max(max(scale.x, scale.y), scale.z);
		return boundingSphere;
	}

	//Items for A3

	//Distance Function
	float GetDistance(XMFLOAT3 input) {
		float distance;
		float preSquare;
		//position.x
		//position.y
		//position.z
		//input.x	
		//input.y	
		//input.z
		preSquare = (((input.x - position.x) * (input.x - position.x)) +
			((input.y - position.y) * (input.y - position.y)) +
			((input.z - position.z) * (input.z - position.z)));

		distance = sqrtf(preSquare);
		
		return distance;
	}

	//Functions needed to move kirby, copied from the Camera.h file and broken down

	//This requires a set forward value
	//The kirby model forward and right values are wrong, so 
	void MoveForward(float amount)
	{
		position.x += forward.x * amount;
		position.y += forward.y * amount;
		position.z += forward.z * amount;
	}

	//We need to get our right value in another function
	void Strafe(float amount)
	{
		position.x += right.x * amount;
		position.y += right.y * amount;
		position.z += right.z * amount;
	}

	//This helps calculate our vectors and keep them in line
	//Normalizes basis vectors (X, Y, Z) and guarantees that they are all perfectly perpendicular. forward vec is normalized but not changed in direction
	static void Orthonormalize(XMVECTOR& fVec, XMVECTOR& rVec, XMVECTOR& uVec)
	{
		//New right vector, XMVector3dot gets the magnitude? XMVectore3LengthSq gets length squared?
		XMVECTOR newRVec = rVec - (XMVector3Dot(rVec, fVec) / XMVector3LengthSq(fVec) * fVec);
		//New Up vector from the previous
		XMVECTOR newUVec = uVec - (XMVector3Dot(uVec, fVec) / XMVector3LengthSq(fVec) * fVec)
			- (XMVector3Dot(uVec, newRVec) / XMVector3LengthSq(newRVec) * newRVec);

		//Renomalize all of them
		XMVector3NormalizeEst(fVec);
		XMVector3NormalizeEst(newRVec);
		XMVector3NormalizeEst(newUVec);
	}


	void RecalculatedUpdate()
	{
		//Notice these always start with the global axiis
		//We could do this the 'continuous' method
		//This is preferrable because we won't accumulate float-precision errors in our rotations.
		//But this won't work for 'free' rotation (like a spaceship) where different 
		//combinations of yaw/pitch/roll can get you to the same orientation
		XMVECTOR forwardVec = XMVectorSet(0, 0, 1, 0);
		XMVECTOR rightVec = XMVectorSet(1, 0, 0, 0);
		XMVECTOR upVec = XMVectorSet(0, 1, 0, 0);

		//Constrain pitch so we can't flip over upside down 
		if (rotation.y > maxPitch)
			rotation.y = maxPitch;
		else if (rotation.y < -maxPitch)
			rotation.y = -maxPitch;

		XMVECTOR yawRotQuaternion = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), rotation.x);
		rightVec = XMVector3Rotate(rightVec, yawRotQuaternion);//Yaw right vector around global y/up vector
		forwardVec = XMVector3Rotate(forwardVec, yawRotQuaternion);//Yaw forward vector around global y vector
		forwardVec = XMVector3Rotate(forwardVec, XMQuaternionRotationAxis(rightVec, rotation.y));//pitch forward vector around local right vector

		//Gimbal Lock.
		//XMVECTOR rollQuat = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), rotation.z);
		//upVec = XMVector3Rotate(upVec, rollQuat);

		//Store results.
		XMStoreFloat3(&forward, forwardVec);
		XMStoreFloat3(&right, rightVec);
		XMStoreFloat3(&up, upVec);

		XMVECTOR posVec = XMLoadFloat3(&position);

		//Removed camera specific code
	}

	void ContinuousUpdate()
	{
		//Load XMVECTORS for doing efficient math things.
		XMVECTOR forwardVec = XMLoadFloat3(&forward);
		XMVECTOR rightVec = XMLoadFloat3(&right);
		XMVECTOR upVec = XMLoadFloat3(&up);

		//Yaw first around current (last frame's) local up vector
		XMVECTOR rotQuat = XMQuaternionRotationAxis(upVec, rotation.x);
		rightVec = XMVector3Rotate(rightVec, rotQuat);
		forwardVec = XMVector3Rotate(forwardVec, rotQuat);

		//Then Pitch around new local right vector
		rotQuat = XMQuaternionRotationAxis(rightVec, rotation.y);
		forwardVec = XMVector3Rotate(forwardVec, rotQuat);
		upVec = XMVector3Rotate(upVec, rotQuat);

		//Then Roll around new local forward vector
		rotQuat = XMQuaternionRotationAxis(forwardVec, rotation.z);
		rightVec = XMVector3Rotate(rightVec, rotQuat);
		upVec = XMVector3Rotate(upVec, rotQuat);

		//Quaternions are vector4's that store rotation
		//as alternatives to Rotation Matrices
		//They are more efficient and avoid gimbal lock when blending
		//between 2 quaternion rotations

		//Gimbal Lock: 
		//is when we lose a degree of freedom (an axis to rotate around)
		//because the combination of rotations we've done makes 2 axis
		//line up on each other (like forward and up are the same)

		//Orthonormalize to prevent accumulation of float-precision errors
		//Orthonormalizing means making sure that all 3 vectors
		//forward, up, and right are perpendicular to each other.
		Orthonormalize(forwardVec, rightVec, upVec);

		//Store results.
		XMStoreFloat3(&forward, forwardVec);
		XMStoreFloat3(&right, rightVec);
		XMStoreFloat3(&up, upVec);

		//Will this cause odd bugs
		//rotation.x = rotation.y = rotation.z = 0;//Reset for next frame

	}
	
	void MoveTo(XMFLOAT3 target, float speed) {

		//We need a thing to store the direction
		XMFLOAT3 direction;
		//Calculate direction
		direction.x = target.x - position.x;
		direction.y = target.y - position.y;
		direction.z = target.z - position.z;

		//Convert the float to a vector
		XMVECTOR directionVector = XMLoadFloat3(&direction);

		
		if (GetDistance(target) < 2) {
			//cout << "Too close";
		}
		else {
			//cout << "Distance: " << GetDistance(target) << "\n";
			//cout << "Got to here" << "\n";
			XMVector3NormalizeEst(directionVector);//This gives us the direction it is in
			//Store results.
			//Swiped from camera update code, we utilize this to convert the vector back?
			XMStoreFloat3(&direction, directionVector);

			//cout << "Direction after nomalizing: X:" << direction.x << " Y: " << direction.y << " Z: " << direction.z << "\n";
			//Finally update position
			position.x += direction.x * speed;
			position.y += direction.y * speed;
			position.z += direction.z * speed;
			//cout << "POSX: " << position.x << " POSY: " << position.y << " POSZ: " << position.z << "\n";

		}
	}

	void Draw(){
		if (!material || !mesh)
		{
			cout << "Error, gameobject missing mesh or material." << endl;
			return;
		}

		//Set CBuffer Data
		Globals::Get().GetGameCBuffer()->objectColor = color;
		Globals::Get().GetGameCBuffer()->objectSpecularity = specIntensity;
		Globals::Get().GetGameCBuffer()->uScale = uvScale.x;
		Globals::Get().GetGameCBuffer()->vScale = uvScale.y;
		
		//This is the wrong multiplication order
		//remember matrix multiplication is non-commutative (order matters)
		//XMMATRIX world =
		//	XMMatrixTranslation(position.x, position.y, position.z)
		//	* XMMatrixScaling(scale.x, scale.y, scale.z)
		//	* XMMatrixRotationRollPitchYaw(rotation.y, rotation.x, rotation.z);
		//XMStoreFloat4x4(&Globals::Get().GetGameCBuffer()->objectTransform,
		//	XMMatrixTranspose(world));
		
		XMMATRIX world = XMMatrixRotationRollPitchYaw(rotation.y, rotation.x, rotation.z)
			* XMMatrixScaling(scale.x, scale.y, scale.z) 
			* XMMatrixTranslation(position.x, position.y, position.z);

		XMStoreFloat4x4(&Globals::Get().GetGameCBuffer()->objectTransform, 
			XMMatrixTranspose(world));

		XMMATRIX temp = world;
		temp.r[3] = XMVectorSet(0, 0, 0, 1);//zero out its translation
		XMVECTOR determinant = XMMatrixDeterminant(temp);
		XMStoreFloat4x4(&Globals::Get().GetGameCBuffer()->inverseTranspose, XMMatrixInverse(&determinant, temp));
			
		//Send CBuffer Data (from GameCBuffer to GPUCBuffer)
		Globals::Get().GetDeviceContext()->UpdateSubresource(
			Globals::Get().GetGPUCBuffer(),
			0, NULL,
			Globals::Get().GetGameCBuffer(),
			0, 0); 
		
		//Draw 
		material->Apply();
		mesh->Draw();
	}
};