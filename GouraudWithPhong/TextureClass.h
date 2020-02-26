#pragma once
#include <d3d11.h>
#include <stdio.h>
#include <stb_image.h>
#include <D3DX11tex.h>

class TextureClass
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	TextureClass();
	~TextureClass();
	bool init(ID3D11Device*, ID3D11DeviceContext*, WCHAR*);
	void stop();

	ID3D11ShaderResourceView* getTexture();

private:
	bool loadTar(char*, int&, int&);
	bool loadImage(char*);

private:
	int m_width, m_height, m_channels;
	unsigned char* m_targaData;
	ID3D11Texture2D *m_texture;
	ID3D11ShaderResourceView *m_textureView;
};