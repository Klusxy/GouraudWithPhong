#pragma once

class InputClass
{
public:
	InputClass();
	~InputClass();

	void init();
	void keyDown(unsigned int);
	void keyUp(unsigned int);

	bool isKeyDown(unsigned int);

private:
	bool m_keys[256];
};