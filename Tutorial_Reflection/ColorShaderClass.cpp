#include "ColorShaderClass.h"

ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_bufferCount = 1;
	m_startNumber = 0;
}

ColorShaderClass::~ColorShaderClass()
{
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{
}


bool ColorShaderClass::Initialize(ID3D11Device* pDevice, HWND hwnd)
{
	HRESULT result;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;
	ID3DBlob* errorMessage;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	unsigned int numElements;

	const wchar_t* vsFilename = L"../ColorVS.hlsl";
	const wchar_t* psFilename = L"../ColorPS.hlsl";

	//�ʱ�ȭ
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//HLSL ������
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		//���Ϸκ��� �����Ͽ� �������� ��� ������ �޽��� �ڽ��� ����
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd);
		}
		//���� ���� �޽����� ���� ��� ������ ã�� �� ���ٴ� �ǹ�
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Vs Texture Shader File", MB_OK);
		}

		return false;
	}

	//HLSL ������
	result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		//���Ϸκ��� �����Ͽ� �������� ��� ������ �޽��� �ڽ��� ����
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd);
		}
		//���� ���� �޽����� ���� ��� ������ ã�� �� ���ٴ� �ǹ�
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Ps Texture Shader File", MB_OK);
		}

		return false;
	}

	//���� ���̴� ���� (������ �� ���̴� ����)
	result = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//�ȼ� ���̴� ���� (������ �� ���̴� ����)
	result = pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";//Semantic ���
	polygonLayout[0].SemanticIndex = 0;//���� Semantic ��� ���� ��ȣ
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;//��� ������ ����
	polygonLayout[0].InputSlot = 0;//0~15
	polygonLayout[0].AlignedByteOffset = 0;//��Ұ� ���۵Ǵ� ��ġ(���۾� �������� ����)
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;//������ ������
	polygonLayout[0].InstanceDataStepRate = 0;//D3D11_INPUT_PER_VERTEX_DATA�� 0���� ����

	polygonLayout[1].SemanticName = "COLOR";//Semantic ���
	polygonLayout[1].SemanticIndex = 0;//���� Semantic ��� ���� ��ȣ
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//��� ������ ����
	polygonLayout[1].InputSlot = 0;//0~15
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;//�ڵ� ���
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;//������ ������
	polygonLayout[1].InstanceDataStepRate = 0;//D3D11_INPUT_PER_VERTEX_DATA�� 0���� ����

	//�Է� �������� ����
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//���� �Է� ���̾ƿ� ����
	result = pDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	//���̴� ���� ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;


	D3D11_BUFFER_DESC matrixBufferDesc;

	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));

	//��Ʈ���� ���� ����
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer0);//���� ũ��
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;//GPU(�б� ����)�� CPU(���� ����)���� �׼��� ���� (�� ������ ���� ������Ʈ)
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//���۸� ��� ���۷� ���ε�
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPU �׼��� ����
	matrixBufferDesc.MiscFlags = 0;//���ҽ��� ���� �÷���
	matrixBufferDesc.StructureByteStride = 0;

	//��Ʈ���� ���� ����
	result = pDevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


bool ColorShaderClass::Render(ID3D11DeviceContext* pDeviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result;

	//���� ������Ʈ
	result = UpdateShaderBuffers(pDeviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//���� ����(�ﰢ��)
	pDeviceContext->IASetInputLayout(m_layout);//�Է� ���̾ƿ� ���ε�
	pDeviceContext->VSSetShader(m_vertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pixelShader, NULL, 0);

	return true;
}

void ColorShaderClass::Shutdown()
{
	//��Ʈ���� ���� ����
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	//���̾ƿ� ����
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	//�ȼ� ���̴� ����
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	//���� ���̴� ����
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}


//���� ������ �޽��� �ڽ��� ���
void ColorShaderClass::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd)
{
	char* compileErrors;

	//���� �޽��� �ؽ�Ʈ ���ۿ� ���� �����͸� ������
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//�޽��� �ڽ� ����
	MessageBoxA(hwnd, compileErrors, "Error", MB_OK);

	//����
	errorMessage->Release();
	errorMessage = 0;

	return;
}

//���̴����� ����ϴ� ���۸� ������Ʈ
bool ColorShaderClass::UpdateShaderBuffers(ID3D11DeviceContext* pDeviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer0* dataPtr = 0;

	//��� ��ġ
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//��� ���۸� ��� (GPU �׼��� ��Ȱ��ȭ) 
	result = pDeviceContext->Map(m_matrixBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//���� ���̴��� ��� ������ �����͸� ����
	dataPtr = (MatrixBuffer0*)mappedResource.pData;

	//�Ű������� ������ �����ͷ� ������Ʈ
	dataPtr->worldMatrix = worldMatrix;
	dataPtr->viewMatrix = viewMatrix;
	dataPtr->projectionMatrix = projectionMatrix;

	//��� ���۸� ��� ���� (GPU �׼��� �ٽ� Ȱ��ȭ)
	pDeviceContext->Unmap(m_matrixBuffer, NULL);

	//���� ���̴��� ��� ���۸� ������Ʈ
	pDeviceContext->VSSetConstantBuffers(m_startNumber, m_bufferCount, &m_matrixBuffer);

	return true;
}