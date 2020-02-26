#include "InputClass.h"

InputClass::InputClass()
{
}

InputClass::~InputClass()
{
}

void InputClass::init()
{
	int i;

	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
}

void InputClass::keyDown(unsigned int input)
{
	m_keys[input] = true;
}

void InputClass::keyUp(unsigned int input)
{
	m_keys[input] = false;
}

bool InputClass::isKeyDown(unsigned int input)
{
	return m_keys[input];
}