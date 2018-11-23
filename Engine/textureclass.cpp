////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureclass.h"


TextureClass::TextureClass()
{
	m_texture = 0;
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}


bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename, bool iscubemap)
{
	HRESULT result;


	// Load the texture in.
	if (iscubemap)
	{
		D3DX11_IMAGE_LOAD_INFO loadSMInfo;
		loadSMInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		ID3D11Texture2D* SMTexture = 0;
		result = D3DX11CreateTextureFromFile(device, filename, &loadSMInfo, 0, (ID3D11Resource**)&SMTexture, 0);
		if (FAILED(result))
		{
			return false;
		}

		D3D11_TEXTURE2D_DESC SMTextureDesc;
		SMTexture->GetDesc(&SMTextureDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
		SMViewDesc.Format = SMTextureDesc.Format;
		SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		SMViewDesc.TextureCube.MipLevels = SMTextureDesc.MipLevels;
		SMViewDesc.TextureCube.MostDetailedMip = 0;

		result = device->CreateShaderResourceView(SMTexture, &SMViewDesc, &m_texture);
		
	}
	else
		result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void TextureClass::Shutdown()
{
	// Release the texture resource.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}


ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}