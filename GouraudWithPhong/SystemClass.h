#pragma once

#define  WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "InputClass.h"
#include "GraphicsClass.h"

class SystemClass
{
public:
	SystemClass();
	~SystemClass();

	bool init();
	void stop();
	void run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool frame();
	void initWindow(int&, int&);
	void stopWindow();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hWnd;

	InputClass *m_input;
	GraphicsClass *m_graphics;

};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass *ApplicationHandle = 0;