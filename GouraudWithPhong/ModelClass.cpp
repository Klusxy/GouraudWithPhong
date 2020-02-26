#include "ModelClass.h"

ModelClass::ModelClass()
{
	m_textureClass = 0;
	m_meshCount = 0;

}

ModelClass::~ModelClass()
{
}

bool ModelClass::init(ID3D11Device* device, ID3D11DeviceContext *deviceContext, WCHAR *textureFilename, char* filename)
{
	bool result;
	m_device = device;
	result = initModelWidthAssimp(filename);
	if (!result)
	{
		return false;
	}

	result = loadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::initModelWidthAssimp(char *filename)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return false;
	}

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[i];
		if (!mesh->HasNormals() || !mesh->HasTextureCoords(0))
		{
			MessageBox(NULL, L"模型文件中没有纹理坐标或者法向信息", L"Error", MB_OK);
			return false;
		}

		VertexType *vertices = new VertexType[mesh->mNumVertices];
		unsigned int *indices = new unsigned int[mesh->mNumFaces * 3];

		for (int j = 0; j < mesh->mNumVertices; j++)
		{
			vertices[j].position = XMFLOAT3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
			vertices[j].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			vertices[j].tex = XMFLOAT2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
			vertices[j].normal = XMFLOAT3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
		}

		for (unsigned int j = 0; j < mesh->mNumFaces; j++)
		{
			aiFace face = mesh->mFaces[j];
			for (int k = 0; k < face.mNumIndices; k++)
				indices[k + j * 3] = face.mIndices[k];
		}

		initBuffer(m_device, vertices, indices, mesh->mNumVertices, mesh->mNumFaces * 3);

		m_meshCount++;
		m_meshIndexCountVector.push_back(mesh->mNumFaces * 3);

		delete[] vertices;
		delete[] indices;
	}

	return true;
}

bool ModelClass::loadTexture(ID3D11Device* device, ID3D11DeviceContext *deviceContext, WCHAR *textureFilename)
{
	bool result;

	m_textureClass = new TextureClass;
	if (!m_textureClass)
	{
		return false;
	}

	result = m_textureClass->init(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::releaseTexture()
{
	if (m_textureClass)
	{
		m_textureClass->stop();
		delete m_textureClass;
		m_textureClass = 0;
	}
}

ID3D11ShaderResourceView* ModelClass::getTexture()
{
	return m_textureClass->getTexture();
}

int ModelClass::getMeshCount()
{
	return m_meshCount;
}

int ModelClass::getIndexCount(int meshIndex)
{
	return m_meshIndexCountVector.at(meshIndex);
}

void ModelClass::stop()
{
	releaseBuffer();

	releaseTexture();
}

void ModelClass::render(ID3D11DeviceContext* deviceContext, int meshIndex)
{
	renderBuffer(deviceContext, meshIndex);
}

bool ModelClass::initBuffer(ID3D11Device* device, VertexType *vertices, unsigned int *indices, int vCount, int iCount)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * iCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	m_vertexBufferVector.push_back(m_vertexBuffer);
	m_indexBufferVector.push_back(m_indexBuffer);

	return true;
}

void ModelClass::releaseBuffer()
{
	for (int i = 0; i < m_vertexBufferVector.size(); i++)
		m_vertexBufferVector.at(i)->Release();

	m_vertexBufferVector.clear();

	for (int i = 0; i < m_indexBufferVector.size(); i++)
		m_indexBufferVector.at(i)->Release();

	m_indexBufferVector.clear();
}

void ModelClass::releaseModel()
{
	m_meshIndexCountVector.clear();
}

void ModelClass::renderBuffer(ID3D11DeviceContext* deviceContext, int meshIndex)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	//input assembler
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBufferVector.at(meshIndex), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBufferVector.at(meshIndex), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}