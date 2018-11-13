
#include "AssimpModelClass.h"


AssimpModelClass::AssimpModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_model = 0;
	m_Texture = NULL;
}


AssimpModelClass::AssimpModelClass(const AssimpModelClass& other)
{
}


AssimpModelClass::~AssimpModelClass()
{
}


bool AssimpModelClass::Initialize(ID3D11Device* device, const char* modelFilename, WCHAR* textureFilename)
{
	bool result;


	// Load in the model data,
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}


	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}


void AssimpModelClass::Shutdown()
{

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	// Release the model texture.
	ReleaseTexture();


	return;
}


void AssimpModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

int AssimpModelClass::GetIndexCount()
{
	return idxVector.size();
}


bool AssimpModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	/*indices = idxVector.data();
	if (!indices)
	{
		return false;
	}*/

	// Load the vertex array and index array with data.
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * idxVector.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = idxVector.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	//delete[] indices;
	indices = 0;

	return true;
}

void AssimpModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void AssimpModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}



bool AssimpModelClass::LoadModel(const char* modelFilename)
{
	const aiScene* scene = aiImportFile(modelFilename, aiProcessPreset_TargetRealtime_MaxQuality 
														| aiProcess_Triangulate 
														/*| aiProcess_MakeLeftHanded */
														| aiProcess_FlipUVs
														| aiProcess_ConvertToLeftHanded);
	if (!scene)
	{
		OutputDebugStringA(aiGetErrorString());
		std::cerr << "Could not load file " << modelFilename << ": " << aiGetErrorString() << std::endl;
		return nullptr;
	}

	m_vertexCount = 0;
	for (std::uint32_t meshIdx = 0u; meshIdx < scene->mNumMeshes; meshIdx++)
	{
		aiMesh* mesh = scene->mMeshes[meshIdx];
		m_vertexCount += mesh->mNumVertices;
	}
	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;
	m_model = new ModelType[m_vertexCount];


	for (std::uint32_t meshIdx = 0u; meshIdx < scene->mNumMeshes; meshIdx++)
	{
		aiMesh* mesh = scene->mMeshes[meshIdx];

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiColor4D specularColour;
		aiColor4D diffuseColour;
		aiColor4D ambientColour;
		float brightness;

		aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specularColour);
		aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColour);
		aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambientColour);
		aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &brightness);

		

		for (int vertIdx = 0; vertIdx < mesh->mNumVertices; vertIdx++)
		{
			aiVector3D vert = mesh->mVertices[vertIdx];
			aiVector3D norm = mesh->mNormals[vertIdx];
			aiVector3D uvs = mesh->mTextureCoords[meshIdx][vertIdx];

			m_model[vertIdx].x = vert.x;
			m_model[vertIdx].y = vert.y;
			m_model[vertIdx].z = vert.z;

			m_model[vertIdx].tu = uvs.x;
			m_model[vertIdx].tv = uvs.y;

			m_model[vertIdx].nx = norm.x;
			m_model[vertIdx].ny = norm.y;
			m_model[vertIdx].nz = norm.z;
		}

		//idxVector.reserve(mesh->mNumFaces * 3u);
		for (std::uint32_t faceIdx = 0u; faceIdx < mesh->mNumFaces; faceIdx++)
		{
			/*idxVector.push_back(mesh->mFaces[faceIdx].mIndices[0u]);
			idxVector.push_back(mesh->mFaces[faceIdx].mIndices[1u]);
			idxVector.push_back(mesh->mFaces[faceIdx].mIndices[2u]);*/
			for (unsigned int j = 0; j < mesh->mFaces[faceIdx].mNumIndices; j++) {
				idxVector.push_back(mesh->mFaces[faceIdx].mIndices[j]);
			}
		}
	}



	aiReleaseImport(scene);
}

void AssimpModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}


ID3D11ShaderResourceView* AssimpModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}


bool AssimpModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void AssimpModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}