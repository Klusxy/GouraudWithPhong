#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class LightClass
{
public:
	LightClass();
	~LightClass();
	void setSpecularColor(float, float, float, float);
	void setAmbientColor(float, float, float, float);
	void setDiffuseColor(float, float, float, float);
	void setLightPosition(float, float, float);

	XMFLOAT3 getLightPosition();
	XMFLOAT4 getDiffuseColor();
	XMFLOAT4 getAmbientColor();
	XMFLOAT4 getSpecularColor();
private:
	XMFLOAT4 m_specularColor;
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_lightPosition;
};