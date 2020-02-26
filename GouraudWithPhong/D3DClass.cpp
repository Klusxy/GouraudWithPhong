#include "D3DClass.h"

D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}

D3DClass::~D3DClass()
{
}

void D3DClass::printInfo(int screenWidth, int screenHeight)
{
	HRESULT hr;
	IDXGIFactory *factory;

	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(hr))
		return;

	IDXGIAdapter *adapter;
	int adapterIndex = 0;
	while (factory->EnumAdapters(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		unsigned long long stringLength;
		hr = adapter->GetDesc(&adapterDesc);
		if (FAILED(hr))
			return;

		char m_videoCardDescription[128];
		int error = wcstombs_s(&stringLength, m_videoCardDescription, adapterDesc.Description, 128);
		if (error)
			return;
		printf("adapter = %d, --    %s   -- \n", adapterIndex, m_videoCardDescription);
		int m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
		printf("adapter = %d, video card memory = %d\n", adapterIndex, m_videoCardMemory);

		int displayIndex = 0;
		IDXGIOutput *adapterOutput;
		while (adapter->EnumOutputs(displayIndex, &adapterOutput) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_MODE_DESC *displayModeList;
			unsigned int numModes, numerator, denominator;

			hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);//有符号归一化整数格式、交错模式

			displayModeList = new DXGI_MODE_DESC[numModes];
			hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);

			for (int i = 0; i < numModes; i++)
			{
				if (displayModeList[i].Width == screenWidth)
				{
					if (displayModeList[i].Height == screenHeight)
					{
						numerator = displayModeList[i].RefreshRate.Numerator;
						denominator = displayModeList[i].RefreshRate.Denominator;
					}
				}
			}
			printf("display = %d, refresh rate = %d\n",displayIndex, numerator / denominator);

			delete[] displayModeList;
			displayModeList = 0;

			adapterOutput->Release();
			adapterOutput = 0;

			displayIndex++;
		}

		adapter->Release();
		adapter = 0;

		adapterIndex++;
	}

	factory->Release();
	factory = 0;
}

bool D3DClass::init(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	//printInfo(screenWidth, screenHeight);

	HRESULT hr;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D *backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	m_vsync_enabled = vsync;

	//swap chain
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (m_vsync_enabled)
	{
		//swapChainDesc.BufferDesc.RefreshRate.Numerator = xxx;
		//swapChainDesc.BufferDesc.RefreshRate.Denominator = xxx;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//将扫描线排序和缩放设置为未指定。
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;//不要设置高级标志。
	featureLevel = D3D_FEATURE_LEVEL_11_0;
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(hr))
		return false;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferPtr);
	if (FAILED(hr))
		return false;
	UINT x4MsaaQuality = 0;
	m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 1, &x4MsaaQuality);
	printf("x4MsaaQuality = %d\n", x4MsaaQuality);
	hr = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(hr))
		return false;
	backBufferPtr->Release();
	backBufferPtr = NULL;

	//depth stencil buffer
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	hr = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(hr))
		return false;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(hr))
		return false;
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;//0->depth stencil not read only
	hr = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(hr))
		return false;
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);//output merger

	//raster state : same as default
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	//rasterDesc.AntialiasedLineEnable = false;//指定是否启用行反锯齿; 仅适用于绘制线条和MultisampleEnable为FALSE的情况。
	//rasterDesc.MultisampleEnable = false;//指定是否在多采样抗锯齿（MSAA）渲染目标上使用四边形或alpha线抗锯齿算法。设置为TRUE以使用四边形线消除锯齿算法，并设置为FALSE以使用alpha线消除锯齿算法
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	//rasterDesc.DepthBias = 0;//深度偏差:在3D空间中共面的多边形可以通过向每一个添加z偏差（或深度偏差）使其看起来好像不共面。
	//rasterDesc.DepthBiasClamp = 0.0f;
	//rasterDesc.SlopeScaledDepthBias = 0.0f;
	//rasterDesc.DepthClipEnable = true;//硬件始终执行光栅化坐标的x和y剪裁。当DepthClipEnable设置为默认值TRUE时，硬件还会剪切 z值
	//rasterDesc.FrontCounterClockwise = false;//false:顺时针为面向前方
	//rasterDesc.ScissorEnable = false;//启用剪刀矩形剔除。活动剪刀矩形外的所有像素都会被删除。
	hr = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(hr))
		return false;
	m_deviceContext->RSSetState(m_rasterState);

	//view port
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.Width = screenWidth;
	viewport.Height = screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	m_deviceContext->RSSetViewports(1, &viewport);

	//
	fieldOfView = 3.141592654f / 8.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;
	//m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	m_worldMatrix = XMMatrixIdentity();

	m_orthoMatrix = XMMatrixOrthographicOffCenterLH(-1.0f, 1.0f, -1.0f, 1.0f, screenNear, screenDepth);

	return true;
}

void D3DClass::stop()
{
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

}

void D3DClass::beginScene(float red, float green, float blue, float alpha)
{
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::endScene()
{
	if (m_vsync_enabled)
	{
		m_swapChain->Present(1, 0);
	}
	else
	{
		m_swapChain->Present(0, 0);
	}
}

ID3D11Device* D3DClass::getDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3DClass::getDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::getProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void D3DClass::getWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}

void D3DClass::getOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}