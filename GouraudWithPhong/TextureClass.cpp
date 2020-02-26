#include "TextureClass.h"

TextureClass::TextureClass()
{
	m_targaData = 0;
	m_texture = 0;
	m_textureView = 0;
	m_width = 0;
	m_height = 0;
	m_channels = 0;
}

TextureClass::~TextureClass()
{
}

bool TextureClass::init(ID3D11Device *device, ID3D11DeviceContext *deviceContext, WCHAR *filename)
{
	bool result;

	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_textureView, NULL);
	if (FAILED(result))
	{
		return false;
	}

	/*D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hr;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	//result = loadTar(filename, width, height);
	result = loadImage(filename);
	if (!result)
	{
		return false;
	}

	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	if (m_channels == 3)
	{
		//textureDesc.Format = D2D1_PIXEL_FORMAT;
		//textureDesc.Format = DXGI_FORMAT_X8R8;
	}
	else if (m_channels == 4)
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	hr = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hr))
	{
		return false;
	}

	rowPitch = m_width * (4 * sizeof(unsigned char));
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	hr = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hr))
	{
		return false;
	}
	deviceContext->GenerateMips(m_textureView);
	delete[] m_targaData;
	m_targaData = 0;*/

	return true;
}

void TextureClass::stop()
{
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}
}

ID3D11ShaderResourceView* TextureClass::getTexture()
{
	return m_textureView;
}

bool TextureClass::loadImage(char*filename)
{
	m_targaData = stbi_load(filename, &m_width, &m_height, &m_channels, 0);
	if (!m_targaData)
	{
		return false;
	}
	return true;
}

//on support 32bit targa
//tga图像上下颠倒
bool TextureClass::loadTar(char* filename, int& height, int& width)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE *fp;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;
	error = fopen_s(&fp, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, fp);
	if (count != 1)
	{
		return false;
	}
	bpp = targaFileHeader.bpp;
	width = targaFileHeader.width;
	height = targaFileHeader.height;
	if (bpp != 32)
	{
		return false;
	}
	imageSize = width * height * 4;

	targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	count = (unsigned int)fread(targaImage, 1, imageSize, fp);
	if (count != imageSize)
	{
		return false;
	}

	error = fclose(fp);
	if (error != 0)
	{
		return false;
	}

	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return false;
	}

	index = 0;

	//tga格式颠倒存储，从最后一行读起
	k = (width * height * 4) - (width * 4);

	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

			k += 4;
			index += 4;
		}

		k -= (width * 8);
	}

	delete[] targaImage;
	targaImage = 0;

	return true;
}