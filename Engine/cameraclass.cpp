////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationPitch = 0.0f;
	m_rotationYaw = 0.0f;
	m_rotationRoll = 0.0f;
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationPitch = x;
	m_rotationYaw = y;
	m_rotationRoll = z;
	return;
}


D3DXVECTOR3 CameraClass::GetPosition()
{
	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
}


D3DXVECTOR3 CameraClass::GetRotation()
{
	return D3DXVECTOR3(m_rotationPitch, m_rotationYaw, m_rotationRoll);
}

D3DXVECTOR3 CameraClass::Strafe(float sign) // + for right, - for left
{
	D3DXVECTOR3 up, lookAt, right;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	
	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	// PI/180 = 0.0174532925f;
	pitch = m_rotationPitch * (D3DX_PI / 180.0);
	yaw = m_rotationYaw * (D3DX_PI / 180.0);
	roll = m_rotationRoll * (D3DX_PI / 180.0);

	// Create the rotation matrix from the yaw, pitch, and roll values.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// LEFT HANDED CROSS PRODUCT
	// https://softwareengineering.stackexchange.com/questions/17519/why-does-directx-use-a-left-handed-coordinate-system
	D3DXVec3Cross(&right, &up, &lookAt);

	D3DXVECTOR3 mov = D3DXVECTOR3(right.x, right.y, right.z);
	D3DXVec3Normalize(&mov, &mov);

	m_positionX += sign * mov.x;
	m_positionY += sign * mov.y;
	m_positionZ += sign * mov.z;

	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);

}

D3DXVECTOR3 CameraClass::Advance(float sign) // + for forward, - for backward
{
	D3DXVECTOR3 lookAt, move;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	// PI/180 = 0.0174532925f;
	pitch = m_rotationPitch * (D3DX_PI / 180.0);
	yaw = m_rotationYaw * (D3DX_PI / 180.0);
	roll = m_rotationRoll * (D3DX_PI / 180.0);

	// Create the rotation matrix from the yaw, pitch, and roll values.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);

	D3DXVECTOR3 mov = D3DXVECTOR3(lookAt.x, lookAt.y, lookAt.z);
	D3DXVec3Normalize(&mov, &mov);

	m_positionX += sign * mov.x;
	m_positionY += sign * mov.y;
	m_positionZ += sign * mov.z;


	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);

}


void CameraClass::Rotate(D3DXVECTOR3 rot)
{
	D3DXVec3Normalize(&rot, &rot);
	m_rotationPitch += rot.y;
	m_rotationYaw += rot.x;
}


void CameraClass::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationPitch * (D3DX_PI / 180.0);
	yaw   = m_rotationYaw * (D3DX_PI / 180.0);
	roll  = m_rotationRoll * (D3DX_PI / 180.0);
	D3DXQUATERNION* q = new D3DXQUATERNION();

	// Create the rotation matrix from the yaw, pitch, and roll values.
	// D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
	D3DXQuaternionRotationYawPitchRoll(q, yaw, pitch, roll);
	D3DXMatrixRotationQuaternion(&rotationMatrix, q);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);

	return;
}


void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}