#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <array>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "TextureClass.h"
using namespace DirectX;
using namespace std;

class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
		XMFLOAT2 tex;
		XMFLOAT3 normal;
	};

public:
	ModelClass();
	~ModelClass();

	bool init(ID3D11Device*, ID3D11DeviceContext*, WCHAR*, char*);
	void stop();
	void render(ID3D11DeviceContext*, int);
	bool initModelWidthAssimp(char*);

	ID3D11ShaderResourceView* getTexture();
	int getMeshCount();
	int getIndexCount(int);

private:
	bool initBuffer(ID3D11Device*, VertexType*, unsigned int*, int, int);
	void releaseBuffer();
	void renderBuffer(ID3D11DeviceContext*, int);

	bool loadTexture(ID3D11Device*, ID3D11DeviceContext*, WCHAR*);
	void releaseTexture();
	void releaseModel();

private:
	int m_meshCount;
	TextureClass *m_textureClass;
	ID3D11Device*m_device;
	vector<int> m_meshIndexCountVector;

	vector<ID3D11Buffer*> m_vertexBufferVector;
	vector<ID3D11Buffer*> m_indexBufferVector;
};