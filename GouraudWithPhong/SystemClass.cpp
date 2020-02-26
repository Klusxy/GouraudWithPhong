#include "SystemClass.h"

SystemClass::SystemClass()
{
	m_input = 0;
	m_graphics = 0;
}

SystemClass::~SystemClass()
{
}

bool SystemClass::init()
{
	int mScreenWidth, mScreenHeight;
	bool result;

	mScreenWidth = 0; mScreenHeight = 0;
	initWindow(mScreenWidth, mScreenHeight);

	m_input = new InputClass;
	if (!m_input)
	{
		return false;
	}
	m_input->init();
	m_graphics = new GraphicsClass;
	if (!m_graphics)
	{
		return false;
	}
	result = m_graphics->init(mScreenWidth, mScreenHeight, m_hWnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::stop()
{
	// Release the graphics object.
	if (m_graphics)
	{
		m_graphics->stop();
		delete m_graphics;
		m_graphics = 0;
	}

	// Release the input object.
	if (m_input)
	{
		delete m_input;
		m_input = 0;
	}

	// Shutdown the window.
	stopWindow();
}

void SystemClass::run()
{
	MSG msg;
	bool done, result;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = frame();
			if (!result)
			{
				done = true;
			}
		}
	}
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// Check if a key has been pressed on the keyboard.
	case WM_KEYDOWN:
	{
		m_input->keyDown((unsigned int)wparam);
		return 0;
	}

	case WM_KEYUP:
	{
		m_input->keyUp((unsigned int)wparam);
		return 0;
	}

	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

bool SystemClass::frame()
{
	bool result;

	if (m_input->isKeyDown(VK_ESCAPE))
	{
		return false;
	}

	result = m_graphics->frame();
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::initWindow(int& w, int& h)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	w = GetSystemMetrics(SM_CXSCREEN);
	h = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)w;
		dmScreenSettings.dmPelsHeight = (unsigned long)h;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		w = 800;
		h = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		//WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		WS_OVERLAPPEDWINDOW,
		posX, posY, w, h, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	// Hide the mouse cursor.
	ShowCursor(true);

	return;
}

void SystemClass::stopWindow()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hWnd);
	m_hWnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}