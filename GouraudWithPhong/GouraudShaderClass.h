#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>
#include <d3dx11async.h>

using namespace DirectX;

class GouraudShaderClass
{

private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

	struct LightBufferType
	{
		XMFLOAT4 specularColor;
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightPosition;
		float padding_light;//USAGE为D3D11_BIND_CONSTANT_BUFFER, 则ByteWidth必须为16的倍数 
	};

public:
	GouraudShaderClass();
	~GouraudShaderClass();
	bool init(ID3D11Device*, HWND);
	void stop();
	bool render(ID3D11DeviceContext*, int, ID3D11ShaderResourceView*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT3, XMFLOAT4, XMFLOAT4, XMFLOAT4, XMFLOAT3);

private:
	bool initShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void stopShader();
	void renderShader(ID3D11DeviceContext*, int, ID3D11ShaderResourceView*);
	void outputShaderErrorMsg(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
	bool setShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT3, XMFLOAT4, XMFLOAT4, XMFLOAT4, XMFLOAT3);
private:
	ID3D11VertexShader *m_vertexShader;
	ID3D11PixelShader *m_pixelShader;
	ID3D11InputLayout *m_inputLayout;
	ID3D11SamplerState *m_sampleState;
	ID3D11Buffer *m_matrixBuffer;
	ID3D11Buffer *m_lightBuffer;
	ID3D11Buffer *m_cameraBuffer;
};