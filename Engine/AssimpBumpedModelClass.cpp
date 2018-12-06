
#include "AssimpBumpedModelClass.h"


AssimpBumpedModelClass::AssimpBumpedModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_model = 0;
	m_TextureArray = NULL;
}


AssimpBumpedModelClass::AssimpBumpedModelClass(const AssimpBumpedModelClass& other)
{
}


AssimpBumpedModelClass::~AssimpBumpedModelClass()
{
}


bool AssimpBumpedModelClass::Initialize(ID3D11Device* device, const char* modelFilename, WCHAR* textureFilename1, WCHAR* textureFilename2, WCHAR* textureFilename3, WCHAR* textureFilename4, WCHAR* textureFilename5)
{
	bool result;


	// Load in the model data,
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}


	// After the model data has been loaded, the new CalculateModelVectors function
	// is called to calculate the tangent and binormal. It also recalculates the normal vector.

	// Calculate the normal, tangent, and binormal vectors for the model.
	CalculateModelVectors();

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}
	
	// The two textures for the model are loaded here.
	// The first is the color texture and the second is the normal map.

	// Load the textures for this model.
	result = LoadTextures(device, textureFilename1, textureFilename2, textureFilename3, textureFilename4, textureFilename5);
	if (!result)
	{
		return false;
	}

	return true;

}


void AssimpBumpedModelClass::Shutdown()
{

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	// Release the model texture.
	ReleaseTextures();


	return;
}


void AssimpBumpedModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

int AssimpBumpedModelClass::GetIndexCount()
{
	return idxVector.size();
}


bool AssimpBumpedModelClass::InitializeBuffers(ID3D11Device* device)
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
	
	// The InitializeBuffers function has changed at this point where the vertex array is loaded with data
	// from the ModelType array. The ModelType array now has tangent and binormal values for the model so 
	// they need to be copied into the vertex array which will then be copied into the vertex buffer.

	// Load the vertex array and index array with data.
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		// New stuff:
		vertices[i].tangent = D3DXVECTOR3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
		vertices[i].binormal = D3DXVECTOR3(m_model[i].bx, m_model[i].by, m_model[i].bz);
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

void AssimpBumpedModelClass::ShutdownBuffers()
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


void AssimpBumpedModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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



bool AssimpBumpedModelClass::LoadModel(const char* modelFilename)
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

void AssimpBumpedModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}


ID3D11ShaderResourceView** AssimpBumpedModelClass::GetTextureArray()
{
	return m_TextureArray->GetTextureArray();
}


bool AssimpBumpedModelClass::LoadTextures(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2, WCHAR* filename3, WCHAR* filename4, WCHAR* filename5)
{
	bool result;


	// Create the texture object.
	m_TextureArray = new TextureArrayClass;
	if (!m_TextureArray)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_TextureArray->Initialize(device, filename1, filename2, filename3, filename4, filename5);
	if (!result)
	{
		return false;
	}

	return true;
}

void AssimpBumpedModelClass::ReleaseTextures()
{
	// Release the texture object.
	if (m_TextureArray)
	{
		m_TextureArray->Shutdown();
		delete m_TextureArray;
		m_TextureArray = 0;
	}

	return;
}


// CalculateModelVectors generates the tangent and binormal for the model as well as a recalculated normal vector
// To start it calculates how many faces(triangles) are in the model
// Then for each of those triangles it gets the three vertices and uses that to calculate the 
// tangent, binormal, and normal
// After calculating those three normal vectors it then saves them back into the model structure.
void AssimpBumpedModelClass::CalculateModelVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;


	// Calculate the number of faces in the model.
	faceCount = m_vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (i = 0; i < faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		vertex1.x = m_model[index].x;
		vertex1.y = m_model[index].y;
		vertex1.z = m_model[index].z;
		vertex1.tu = m_model[index].tu;
		vertex1.tv = m_model[index].tv;
		vertex1.nx = m_model[index].nx;
		vertex1.ny = m_model[index].ny;
		vertex1.nz = m_model[index].nz;
		index++;

		vertex2.x = m_model[index].x;
		vertex2.y = m_model[index].y;
		vertex2.z = m_model[index].z;
		vertex2.tu = m_model[index].tu;
		vertex2.tv = m_model[index].tv;
		vertex2.nx = m_model[index].nx;
		vertex2.ny = m_model[index].ny;
		vertex2.nz = m_model[index].nz;
		index++;

		vertex3.x = m_model[index].x;
		vertex3.y = m_model[index].y;
		vertex3.z = m_model[index].z;
		vertex3.tu = m_model[index].tu;
		vertex3.tv = m_model[index].tv;
		vertex3.nx = m_model[index].nx;
		vertex3.ny = m_model[index].ny;
		vertex3.nz = m_model[index].nz;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		// Store the normal, tangent, and binormal for this face back in the model structure.
		m_model[index - 1].nx = normal.x;
		m_model[index - 1].ny = normal.y;
		m_model[index - 1].nz = normal.z;
		m_model[index - 1].tx = tangent.x;
		m_model[index - 1].ty = tangent.y;
		m_model[index - 1].tz = tangent.z;
		m_model[index - 1].bx = binormal.x;
		m_model[index - 1].by = binormal.y;
		m_model[index - 1].bz = binormal.z;

		m_model[index - 2].nx = normal.x;
		m_model[index - 2].ny = normal.y;
		m_model[index - 2].nz = normal.z;
		m_model[index - 2].tx = tangent.x;
		m_model[index - 2].ty = tangent.y;
		m_model[index - 2].tz = tangent.z;
		m_model[index - 2].bx = binormal.x;
		m_model[index - 2].by = binormal.y;
		m_model[index - 2].bz = binormal.z;

		m_model[index - 3].nx = normal.x;
		m_model[index - 3].ny = normal.y;
		m_model[index - 3].nz = normal.z;
		m_model[index - 3].tx = tangent.x;
		m_model[index - 3].ty = tangent.y;
		m_model[index - 3].tz = tangent.z;
		m_model[index - 3].bx = binormal.x;
		m_model[index - 3].by = binormal.y;
		m_model[index - 3].bz = binormal.z;
	}

	return;
}



// The CalculateTangentBinormal function takes in three vertices and then calculates and returns the 
// tangent and binormal of those three vertices.
void AssimpBumpedModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3,
	VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}


// The CalculateNormal function takes in the tangent and binormal and then does a cross product
// to give back the normal vector.
void AssimpBumpedModelClass::CalculateNormal(VectorType tangent, VectorType binormal, VectorType& normal)
{
	float length;


	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// Calculate the length of the normal.
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// Normalize the normal.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
}
