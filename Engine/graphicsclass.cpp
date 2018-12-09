////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_SkyShader = 0;
	m_Light = 0;
	m_BumpMapShader = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_camPos = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_Camera->SetPosition(m_camPos.x, m_camPos.y, m_camPos.z);


	// Create the model object.
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/cube.txt", L"../Engine/data/grassfloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
		return false;
	}

	// Create the model object.
	m_ModelSky = new ModelClass;
	if (!m_ModelSky)
	{
		return false;
	}

	// Initialize the model object.
	result = m_ModelSky->Initialize(m_D3D->GetDevice(), "../Engine/data/sphere.txt", L"../Engine/Assets/Skybox/skybox_texture.dds", true);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the sky model object.", L"Error", MB_OK);
		return false;
	}

	m_ModelSword = new AssimpModelClass;
	if (!m_ModelSword)
	{
		return false;
	}
	result = m_ModelSword->Initialize(m_D3D->GetDevice(), "../Engine/Assets/claymore/Mini_claymore.obj", L"../Engine/Assets/claymore/Mini_claymore_texture.png");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the sword model object.", L"Error", MB_OK);
		return false;
	}

	// Old Rock
	/*m_ModelRock = new AssimpModelClass;
	if (!m_ModelRock)
	{
		return false;
	}
	result = m_ModelRock->Initialize(m_D3D->GetDevice(), "../Engine/Assets/rock/stone.obj", L"../Engine/Assets/rock/tex/stone_albedo.png");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}*/

	// New Rock
	m_ModelRock = new AssimpBumpedModelClass;
	if (!m_ModelRock)
	{
		return false;
	}
	result = m_ModelRock->Initialize(m_D3D->GetDevice(), "../Engine/Assets/rock/stone.obj", 
														L"../Engine/Assets/rock/tex/stone_albedo.png", 
														L"../Engine/Assets/rock/tex/stone_normal.png",
														L"../Engine/Assets/rock/tex/stone_diffuse.png",
														L"../Engine/Assets/rock/tex/stone_ao.png", 
														L"../Engine/Assets/rock/tex/stone_specular.png");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the rock model object.", L"Error", MB_OK);
		return false;
	}



	// Here we create and initialize the BumpMapShaderClass object.

	// Create the bump map shader object.
	m_BumpMapShader = new BumpMapShaderClass;
	if (!m_BumpMapShader)
	{
		return false;
	}

	// Initialize the bump map shader object.
	result = m_BumpMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
		return false;
	}



	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}


	// Create the sky shader object.
	m_SkyShader = new LightShaderClass;
	if (!m_SkyShader)
	{
		return false;
	}

	result = m_SkyShader->Initialize(m_D3D->GetDevice(), hwnd, L"../Engine/sky.vs", L"../Engine/sky.ps");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the sky light shader object.", L"Error", MB_OK);
		return false;
	}


	// Create the light object.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColour(1.f, 1.0f, 1.f, 1.0f);
	m_Light->SetDiffuseColour(.6f, .0f, .3f, 1.0f);
	m_Light->SetSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);

	// Create the render to texture object.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the light shader object.
	if (m_SkyShader)
	{
		m_SkyShader->Shutdown();
		delete m_SkyShader;
		m_SkyShader = 0;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the model object.
	if (m_ModelSky)
	{
		m_ModelSky->Shutdown();
		delete m_ModelSky;
		m_ModelSky = 0;
	}

	// Release the model object.
	if (m_ModelSword)
	{
		m_ModelSword->Shutdown();
		delete m_ModelSword;
		m_ModelSword = 0;
	}

	// Release the model object.
	if (m_ModelRock)
	{
		m_ModelRock->Shutdown();
		delete m_ModelRock;
		m_ModelRock = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	// Release the render to texture object.
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// Release the bump map shader object.
	if (m_BumpMapShader)
	{
		m_BumpMapShader->Shutdown();
		delete m_BumpMapShader;
		m_BumpMapShader = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;
	static float rotation = 0.0f;
	static float delta = 0.0f;


	// Update the rotation variable each frame.
	rotation += (float)D3DX_PI * 0.01f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Update the delta variable each frame. (keep this between 0 and 1)
	delta += 0.001;
	if (delta > 100000000.0f)
	{
		delta -= 100000000.0f;
	}

	// Render the graphics scene.
	result = Render(rotation, delta);
	if (!result)
	{
		return false;
	}

	return true;
}


void GraphicsClass::Strafe(float sign)
{
	m_camPos = m_Camera->Strafe(sign);
}

void GraphicsClass::Advance(float sign)
{
	 m_camPos = m_Camera->Advance(sign);
}

void GraphicsClass::Rotate(D3DXVECTOR3 rot)
{
	m_Camera->Rotate(rot);
}


bool GraphicsClass::Render(float rotation, float deltavalue)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Render the entire scene to the texture first.
	/*result = RenderToTexture();
	if (!result)
	{
		return false;
	}*/

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 0.0f);

	// Render the scene as normal to the back buffer.
	/*result = RenderScene();
	if (!result)
	{
		return false;
	}*/

	/*D3DXVECTOR3 light = m_Light->GetDirection();
	light.x = cos(30 * deltavalue);
	light.z = sin(30 * deltavalue);*/
	m_Light->SetDirection(cos(30), sin(30), 0.0f);

	/*D3DXVECTOR3 cam = { -cos(30 * deltavalue) + 0.0f, -sin(30 * deltavalue) + 0.0f,  -10.0f };
	m_Camera->SetPosition(cam[0], cam[1], cam[2]);*/


	D3DXMATRIX transform;
	D3DXMatrixIdentity(&transform);
	D3DXMATRIX rotx;
	D3DXMatrixIdentity(&transform);
	D3DXMATRIX roty;
	D3DXMatrixIdentity(&transform);
	D3DXMATRIX rotz;
	D3DXMatrixIdentity(&transform);



	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Rotate the world matrix by the rotation value so that the model will spin.
	// D3DXMatrixRotationY(&worldMatrix, rotation);
	
	// Scaling
	transform._11 = 20.0f;
	transform._22 = 0.01f;
	transform._33 = 20.0f;
	// Translation
	transform._41 = 0.0f;
	transform._42 = -2.0f;
	transform._43 = 0.0f;
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &transform);


	m_Model->Render(m_D3D->GetDeviceContext());

	// Render the model using the light shader.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Light->GetDirection(), m_Light->GetAmbientColour(), m_Light->GetDiffuseColour(), m_Camera->GetPosition(),
		m_Light->GetSpecularColour(), m_Light->GetSpecularPower(), deltavalue, m_Model->GetTexture());
	if (!result)
	{
		return false;
	}


	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);

	D3DXMatrixIdentity(&transform);
	
	D3DXMatrixRotationZ(&rotz, 45 * (D3DX_PI / 180.0f));
	D3DXMatrixRotationY(&roty, 90 * (D3DX_PI / 180.0f));

	D3DXMatrixMultiply(&transform, &roty, &rotz);
	// Translation
	transform._41 = 0.5f;
	transform._42 = 1.4f;
	transform._43 = -0.2f;
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &transform);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_ModelSword->Render(m_D3D->GetDeviceContext());


	// Render the model using the light shader.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_ModelSword->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Light->GetDirection(), m_Light->GetAmbientColour(), m_Light->GetDiffuseColour(), m_Camera->GetPosition(),
		m_Light->GetSpecularColour(), m_Light->GetSpecularPower(), deltavalue, /*m_RenderTexture->GetShaderResourceView()*/m_ModelSword->GetTexture());
	if (!result)
	{
		return false;
	}



	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);

	D3DXMatrixIdentity(&transform);
	// Scaling
	transform._11 = 3.0f;
	transform._22 = 3.0f;
	transform._33 = 3.0f;
	// Translation
	transform._41 = 0.0f;
	transform._42 = -0.6f;
	transform._43 = 0.0f;
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &transform);

	D3DXVECTOR4 v = m_Light->GetSpecularColour();
	v.x = v.y = v.z = v.w = 0.0f;

	m_ModelRock->Render(m_D3D->GetDeviceContext());
	m_Light->SetDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);

	// Render the model using the light shader.
	/*result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_ModelRock->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Light->GetDirection(), m_Light->GetAmbientColour(), m_Light->GetDiffuseColour(), m_Camera->GetPosition(),
		m_Light->GetSpecularColour(), m_Light->GetSpecularPower(), deltavalue, m_ModelRock->GetTexture());*/
	result = m_BumpMapShader->Render(m_D3D->GetDeviceContext(), m_ModelRock->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_ModelRock->GetTextureArray(), m_Light->GetDirection());
	if (!result)
	{
		return false;
	}


	m_D3D->GetWorldMatrix(worldMatrix);

	// Translation
	transform._41 = m_camPos.x;
	transform._42 = m_camPos.y;
	transform._43 = m_camPos.z;
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &transform);


	m_D3D->setSkyMode(true);

	m_ModelSky->Render(m_D3D->GetDeviceContext());

	// Render the model using the light shader.
	result = m_SkyShader->Render(m_D3D->GetDeviceContext(), m_ModelSky->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Light->GetDirection(), m_Light->GetAmbientColour(), m_Light->GetDiffuseColour(), m_Camera->GetPosition(),
		m_Light->GetSpecularColour(), m_Light->GetSpecularPower(), deltavalue, m_ModelSky->GetTexture());
	if (!result)
	{
		return false;
	}


	m_D3D->setSkyMode(false);



	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}


bool GraphicsClass::RenderToTexture()
{
	bool result;


	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Render the scene now and it will draw to the render to texture instead of the back buffer.
	result = RenderScene();
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}


bool GraphicsClass::RenderScene()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;
	static float rotation = 0.0f;


	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Update the rotation variable each frame.
	/*rotation += (float)D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}*/

	D3DXMatrixRotationY(&worldMatrix, rotation);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_D3D->GetDeviceContext());

	// Render the model using the light shader.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Light->GetDirection(), m_Light->GetAmbientColour(), m_Light->GetDiffuseColour(), m_Camera->GetPosition(),
		m_Light->GetSpecularColour(), m_Light->GetSpecularPower(), 0.0, m_Model->GetTexture());
	if (!result)
	{
		return false;
	}

	return true;
}