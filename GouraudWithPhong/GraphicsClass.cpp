#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	m_direct3DClass = 0;
	m_modelClass = 0;
	m_cameraClass = 0;
	m_phongShaderClass = 0;
	m_gouraudShaderClass = 0;
	m_lightClass = 0;
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::init(int screenWidth, int screenHeight, HWND hWnd)
{
	bool result;

	m_direct3DClass = new D3DClass;
	if (!m_direct3DClass)
	{
		return false;
	}
	result = m_direct3DClass->init(screenWidth, screenHeight, VSYNC_ENABLED, hWnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hWnd, L"d3d11 init error", L"Error", MB_OK);
		return false;
	}

	m_cameraClass = new CameraClass;
	m_cameraClass->setPosition(0.0f, 0.0f, -10.0f);

	m_modelClass = new ModelClass;
	if (!m_modelClass)
	{
		return false;
	}
	result = m_modelClass->init(m_direct3DClass->getDevice(), m_direct3DClass->getDeviceContext(),
								L"", "../Models/Sphere_64.obj");
	if (!result)
	{
		MessageBox(hWnd, L"model init error", L"Error", MB_OK);
		return false;
	}

	m_phongShaderClass = new PhongShaderClass;
	if (!m_phongShaderClass)
	{
		return false;
	}
	result = m_phongShaderClass->init(m_direct3DClass->getDevice(), hWnd);
	if (!result)
	{
		MessageBox(hWnd, L"shader init error", L"Error", MB_OK);
		return false;
	}

	m_gouraudShaderClass = new GouraudShaderClass;
	if (!m_gouraudShaderClass)
	{
		return false;
	}
	result = m_gouraudShaderClass->init(m_direct3DClass->getDevice(), hWnd);
	if (!result)
	{
		MessageBox(hWnd, L"shader init error", L"Error", MB_OK);
		return false;
	}

	m_lightClass = new LightClass;
	if (!m_lightClass)
	{
		return false;
	}
	m_lightClass->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_lightClass->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_lightClass->setAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_lightClass->setLightPosition(-1.0f, 1.3f, -2.0f); //为了比较gouraud和phong的区别，所以不用cb中的lightPosition

	return true;
}

void GraphicsClass::stop()
{
	if (m_direct3DClass)
	{
		m_direct3DClass->stop();
		delete m_direct3DClass;
		m_direct3DClass = 0;
	}

	if (m_modelClass)
	{
		m_modelClass->stop();
		delete m_modelClass;
		m_modelClass = 0;
	}

	if (m_cameraClass)
	{
		delete m_cameraClass;
		m_cameraClass = 0;
	}

	if (m_phongShaderClass)
	{
		m_phongShaderClass->stop();
		delete m_phongShaderClass;
		m_phongShaderClass = 0;
	}

	if (m_gouraudShaderClass)
	{
		m_gouraudShaderClass->stop();
		delete m_gouraudShaderClass;
		m_gouraudShaderClass = 0;
	}

	if (m_lightClass)
	{
		delete m_lightClass;
		m_lightClass = 0;
	}
}

bool GraphicsClass::frame()
{
	bool result;
	static float rotation = 0.0f;

	rotation += XM_PI * 0.0001f;
	if (rotation > 360.0f)
		rotation = 0.0f;
	
	result = render(rotation);
	if (!result)
	{
		return false;
	}
	return true;
}

bool GraphicsClass::render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;
	m_direct3DClass->beginScene(0.0f, 0.0f, 0.0f, 1.0f);
	m_cameraClass->render();

	m_direct3DClass->getWorldMatrix(worldMatrix);
	m_cameraClass->getViewMatrix(viewMatrix);
	m_direct3DClass->getProjectionMatrix(projectionMatrix);
	//m_direct3D->getOrthoMatrix(projectionMatrix);

	//XMConvertToRadians(0.0f)
	XMMATRIX rotationMatrix = XMMatrixRotationY(rotation);
	XMMATRIX translateMatrix_phong = XMMatrixTranslation(1.2f, 0.0f, 0.0f);

	for (int i = 0; i < m_modelClass->getMeshCount(); i++)
	{
		m_modelClass->render(m_direct3DClass->getDeviceContext(), i);

		result = m_phongShaderClass->render(m_direct3DClass->getDeviceContext(), m_modelClass->getIndexCount(i), m_modelClass->getTexture(),
			translateMatrix_phong, viewMatrix, projectionMatrix,
			m_lightClass->getLightPosition(), m_lightClass->getDiffuseColor(), m_lightClass->getAmbientColor(), m_lightClass->getSpecularColor(),
			XMFLOAT3(0.0f, 0.0f, 1.0f));
		if (!result)
		{
			return false;
		}
	}

	XMMATRIX translateMatrix_gouraud = XMMatrixTranslation(-1.2f, 0.0f, 0.0f);
	for (int i = 0; i < m_modelClass->getMeshCount(); i++)
	{
		m_modelClass->render(m_direct3DClass->getDeviceContext(), i);

		result = m_gouraudShaderClass->render(m_direct3DClass->getDeviceContext(), m_modelClass->getIndexCount(i), m_modelClass->getTexture(),
			translateMatrix_gouraud, viewMatrix, projectionMatrix,
			m_lightClass->getLightPosition(), m_lightClass->getDiffuseColor(), m_lightClass->getAmbientColor(), m_lightClass->getSpecularColor(),
			XMFLOAT3(0.0f, 1.0f, 0.0f));
		//m_cameraClass->getPosition()
		if (!result)
		{
			return false;
		}
	}

	m_direct3DClass->endScene();
	return true;
}