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

#include "texturearrayclass.h"

// TODO: Make inherit from ModelClass, FFS
class AssimpBumpedModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};
	
	// The following two structures will be used for 
	// calculating the tangent and binormal.
	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	AssimpBumpedModelClass();
	AssimpBumpedModelClass(const AssimpBumpedModelClass&);
	~AssimpBumpedModelClass();

	bool Initialize(ID3D11Device* device, const char* modelFilename, WCHAR* textureFilename1, WCHAR* etc, WCHAR*, WCHAR*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);
	
	int GetIndexCount();
	ID3D11ShaderResourceView** GetTextureArray();
	
private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	bool LoadTextures(ID3D11Device* device, WCHAR* filename1, WCHAR* etc, WCHAR*, WCHAR*, WCHAR*);
	void ReleaseTextures();

	bool LoadModel(const char* modelFilename);
	void ReleaseModel();

	
	// We have three new functions for 
	// calculating the tangent and binormal vectors
	// for the model.

	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	std::vector<unsigned int> idxVector;
	ModelType* m_model;
	TextureArrayClass* m_TextureArray;


};