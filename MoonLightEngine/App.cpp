#include "App.h"

App::App() : _window(800, 600, L"Moonlight Engine")
{
	//this->_window.CreateButton(this->_window.getHWND());
}

App::~App()
{
}

int App::Go()
{
	while (true)
	{
		if (const auto ecode = Window::ProssessMessages())
		{
			return *ecode;
		}
		// execute the game logic
		const auto dt = _timer.Mark() * _speedFactor;

		doFrame();
	}
}

void App::doFrame()
{
	this->_window.getGfx().ClearBuffer(0,0,0);
	this->_window.getGfx().DrawTestTriangle();
	this->_window.getGfx().EndFrame();
}
