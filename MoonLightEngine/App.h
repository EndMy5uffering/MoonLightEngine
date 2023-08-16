#pragma once
#include "Window.h"
#include "Timer.h"
#include "Utils.h"

class App
{
public:
	App();
	~App();
	int Go();
private:
	void doFrame();
private:
	Window _window;
	Timer _timer;
	float _speedFactor = 1.0f;
};

