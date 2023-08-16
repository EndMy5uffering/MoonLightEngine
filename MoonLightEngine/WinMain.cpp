#include "Window.h"
#include "App.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) 
{
	try 
	{
		return App{}.Go();
	}
	catch (const MLException& e) 
	{
		MessageBox(nullptr, UTIL::charToWchar_t(e.what()),
			UTIL::charToWchar_t(e.GetType()), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e) 
	{
		MessageBox(nullptr, UTIL::charToWchar_t(e.what()), L"General exception", MB_OK);
	}
	catch (...) 
	{
		MessageBox(nullptr, L"No details available!", L"Unknown excepiton!", MB_OK);
	}
	return -1;
}