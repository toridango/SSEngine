////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////
class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetDiffuseColour(float, float, float, float);
	void SetAmbientColour(float, float, float, float);
	void SetSpecularColour(float, float, float, float);
	void SetSpecularPower(float);
	void SetDirection(float, float, float);

	D3DXVECTOR4 GetDiffuseColour();
	D3DXVECTOR4 GetAmbientColour();
	D3DXVECTOR4 GetSpecularColour();
	float GetSpecularPower();
	D3DXVECTOR3 GetDirection();

private:
	D3DXVECTOR4 m_diffuseColour;
	D3DXVECTOR4 m_ambientColour;
	D3DXVECTOR4 m_specularColour;
	float m_specularPower;
	D3DXVECTOR3 m_direction;
};

#endif