#include "LightClass.h"

LightClass::LightClass()
{
}

LightClass::~LightClass()
{
}

void LightClass::setDiffuseColor(float r, float g, float b, float a)
{
	m_diffuseColor = XMFLOAT4(r, g, b, a);
}

void LightClass::setLightPosition(float x, float y, float z)
{
	m_lightPosition = XMFLOAT3(x, y, z);
}

void LightClass::setAmbientColor(float r, float g, float b, float a)
{
	m_ambientColor = XMFLOAT4(r, g, b, a);
}

void LightClass::setSpecularColor(float r, float g, float b, float a)
{
	m_specularColor = XMFLOAT4(r, g, b, a);
}

XMFLOAT4 LightClass::getSpecularColor()
{
	return m_specularColor;
}

XMFLOAT4 LightClass::getAmbientColor()
{
	return m_ambientColor;
}

XMFLOAT3 LightClass::getLightPosition()
{
	return m_lightPosition;
}

XMFLOAT4 LightClass::getDiffuseColor()
{
	return m_diffuseColor;
}