#pragma once
#include "D3DClass.h"
#include "PhongShaderClass.h"
#include "GouraudShaderClass.h"
#include "ModelClass.h"
#include "CameraClass.h"
#include "LightClass.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass();
	~GraphicsClass();
	bool init(int, int, HWND);
	void stop();
	bool frame();

private:
	bool render(float);

private:
	D3DClass *m_direct3DClass;
	PhongShaderClass *m_phongShaderClass;
	GouraudShaderClass *m_gouraudShaderClass;
	ModelClass *m_modelClass;
	CameraClass *m_cameraClass;
	LightClass *m_lightClass;
};