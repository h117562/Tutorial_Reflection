#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_CameraClass = 0;
	m_uiManager = 0;
	m_TextClass = 0;
	m_ShaderManager = 0;

	m_renderTexture = 0;
	m_rectangle = 0;
	m_mirror = 0;
}

ApplicationClass::~ApplicationClass()
{
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}

bool ApplicationClass::Initialize(HWND hwnd)
{
	bool result;

	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	//DirectX 초기화
	result = m_Direct3D->Initialize(hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	m_ShaderManager = new ShaderManager;
	if (!m_ShaderManager)
	{
		return false;
	}

	//쉐이더 매니저 초기화
	result = m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ShaderManager.", L"Error", MB_OK);
		return false;
	}

	m_TextClass = new TextClass;
	if (!m_TextClass)
	{
		return false;
	}

	//TextClass 초기화
	result = m_TextClass->Initialize(m_Direct3D->GetSwapChain());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TextClass.", L"Error", MB_OK);
		return false;
	}

	m_uiManager = new UIManager;
	if (!m_uiManager)
	{
		return false;
	}

	//UI 매니저 초기화
	result = m_uiManager->Initialize(m_Direct3D);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the UI Manager.", L"Error", MB_OK);
		return false;
	}

	m_CameraClass = new CameraClass;
	if (!m_CameraClass)
	{
		return false;
	}
	
	//기본 뷰 매트릭스 초기화
	m_CameraClass->SetBaseViewMatrix();


	{
		m_renderTexture = new RenderTextureClass;
		if (!m_renderTexture)
		{
			return false;
		}

		result = m_renderTexture->Initialize(m_Direct3D->GetDevice(), 512, 512, SCREEN_FAR, SCREEN_NEAR);
		if (!result)
		{
			MessageBox(hwnd, L"Could not Initialize RenderTextureClass", L"Error", MB_OK);
			return false;
		}

		m_rectangle = new RectangleModel;
		if (!m_rectangle)
		{
			return false;
		}

		result = m_rectangle->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
		if (!result)
		{
			MessageBox(hwnd, L"Rect Initialize Failed", L"Error", MB_OK);
			return false;
		}

		m_mirror = new MirrorModel;
		if (!m_mirror)
		{
			return false;
		}

		result = m_mirror->Initialize(m_Direct3D->GetDevice());
		if (!result)
		{
			MessageBox(hwnd, L"Mirror Initialize Failed", L"Error", MB_OK);
			return false;
		}
	}

	return result;
}

void ApplicationClass::Shutdown()
{
	{
		if (m_mirror)
		{
			m_mirror->Shutdown();
			delete m_mirror;
			m_mirror = 0;
		}

		if (m_rectangle)
		{
			m_rectangle->Shutdown();
			delete m_rectangle;
			m_rectangle = 0;
		}

		if (m_renderTexture)
		{
			m_renderTexture->Shutdown();
			delete m_renderTexture;
			m_renderTexture = 0;
		}
	}


	if (m_uiManager)
	{
		m_uiManager->Shutdown();
		delete m_uiManager;
		m_uiManager = 0;
	}

	if (m_TextClass)
	{
		m_TextClass->Shutdown();
		delete m_TextClass;
		m_TextClass = 0;
	}

	if (m_CameraClass)
	{
		delete m_CameraClass;
		m_CameraClass = 0;
	}

	if (m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}

	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}

bool ApplicationClass::Frame(HWND hwnd, InputClass* pInputClass, FrameTimer* pFrameTimer)
{
	bool result;

	//사용자 입력 처리
	HandleInput(pInputClass, pFrameTimer);

	//그래픽 렌더링
	Render(hwnd, pInputClass);

	return true;
}

void ApplicationClass::HandleInput(InputClass* pInputClass, FrameTimer* pFrameTimer)
{
	bool state;
	float deltaX, deltaY, frameTime;

	frameTime = pFrameTimer->GetTime();
	m_CameraClass->SetFrameTime(frameTime);

	state = pInputClass->GetKeyPressed(DIK_A);
	m_CameraClass->MoveLeft(state);

	state = pInputClass->GetKeyPressed(DIK_S);
	m_CameraClass->MoveBackward(state);

	state = pInputClass->GetKeyPressed(DIK_D);
	m_CameraClass->MoveRight(state);

	state = pInputClass->GetKeyPressed(DIK_W);
	m_CameraClass->MoveForward(state);

	pInputClass->GetMouseDelta(deltaX, deltaY);
	m_CameraClass->UpdateRotation(deltaX, deltaY);

	return;
}


void ApplicationClass::Render(HWND hwnd, InputClass* pInputClass)
{
	m_CameraClass->Render();

	//{
	//	m_renderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
	//	m_renderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.3f, 1.0f);


	//	XMMATRIX world, view, proj;

	//	m_Direct3D->GetWorldMatrix(world);
	//	m_renderTexture->GetProjectionMatrix(proj);
	//	m_CameraClass->GetViewMatrix(view);

	//	world = XMMatrixIdentity();

	//	m_ShaderManager->GetTextureShader()->Render(m_Direct3D->GetDeviceContext(), world, view, proj);
	//	m_rectangle->Render(m_Direct3D->GetDeviceContext());

	//	m_Direct3D->ResetRenderTarget();
	//}

	//3D RenderTarget 초기화(특정 컬러로)
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.2f, 1.0f);

	//2D RenderTarget 초기화
	m_TextClass->BeginDraw();

	{
		XMMATRIX rectWorld, view, proj;
		
		m_Direct3D->GetProjectionMatrix(proj);
		m_CameraClass->GetViewMatrix(view);
		rectWorld = XMMatrixIdentity() * XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		m_ShaderManager->GetTextureShader()->Render(m_Direct3D->GetDeviceContext(), rectWorld, view, proj);
		m_rectangle->Render(m_Direct3D->GetDeviceContext());


		m_Direct3D->TurnMask();

		XMMATRIX mirrorWorld;

		mirrorWorld = XMMatrixIdentity() * XMMatrixRotationX(XM_PI/2) * XMMatrixTranslation(0.0f, -1.0f, 0.0f) * XMMatrixScaling(2,2,2);

		m_ShaderManager->GetTextureShader()->Render(m_Direct3D->GetDeviceContext(), mirrorWorld, view, proj);
		m_mirror->Render(m_Direct3D->GetDeviceContext(), nullptr);

		XMVECTOR pos1, pos2, pos3;

		pos1 = XMVectorSet(-1.0f, -1.0f, 0.0f, 0);
		pos2 = XMVectorSet(-1.0f, 1.0f, 0.0f, 0);
		pos3 = XMVectorSet(1.0f, -1.0f, 0.0f, 0);

		pos1 = XMVector3Transform(pos1, mirrorWorld);
		pos2 = XMVector3Transform(pos2, mirrorWorld);
		pos3 = XMVector3Transform(pos3, mirrorWorld);

		XMVECTOR plane = XMPlaneFromPoints(
			pos1,
			pos2,
			pos3
		);
		
		m_Direct3D->TurnMirror();

		XMMATRIX reflectMat = rectWorld * XMMatrixReflect(plane);

		m_ShaderManager->GetTextureShader()->Render(m_Direct3D->GetDeviceContext(), reflectMat, view, proj);
		m_rectangle->Render(m_Direct3D->GetDeviceContext());
	
		m_Direct3D->TurnZBufferOn();

	}


	//UI 렌더링
	m_uiManager->Frame(m_Direct3D, hwnd, m_ShaderManager, m_TextClass, m_CameraClass, pInputClass);

	m_TextClass->EndDraw();
	m_Direct3D->EndScene();

	return;
}