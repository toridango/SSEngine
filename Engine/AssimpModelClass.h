#pragma once

#include <vector>
#include <memory>
#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include <iostream>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include "textureclass.h"

// TODO: Make inherit from ModelClass, FFS
class AssimpModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	AssimpModelClass();
	AssimpModelClass(const AssimpModelClass&);
	~AssimpModelClass();

	bool Initialize(ID3D11Device* device, const char* modelFilename, WCHAR* textureFilename);
	bool InitializeBuffers(ID3D11Device* device);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);
	int GetIndexCount();
	
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	bool LoadModel(const char* modelFilename);
	void ReleaseModel();

	ID3D11ShaderResourceView* GetTexture();
	bool LoadTexture(ID3D11Device* device, WCHAR* filename);
	void ReleaseTexture();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	std::vector<unsigned int> idxVector;
	ModelType* m_model;
	TextureClass* m_Texture;


};