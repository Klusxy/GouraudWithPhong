#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "SystemClass.h"

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	SystemClass *mSystemClass;
	bool result;

	mSystemClass = new SystemClass;
	if (!mSystemClass)
	{
		return 0;
	}

	result = mSystemClass->init();
	if (result)
	{
		mSystemClass->run();
	}
	mSystemClass->stop();
	delete mSystemClass;
	mSystemClass = 0;

	getchar();
	return 0;
}