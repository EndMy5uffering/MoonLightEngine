#pragma once

#include "MyWin.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Timer.h"
#include "Graphics.h"
#include "MLException.h"
#include <system_error>
#include <optional>
#include <memory>
#include "resource.h"

#define WIN_EXEPT(errorcode) Window::WindowException(__LINE__, __FILE__, errorcode);
#define WIN_EXEPT_LAST() Window::WindowException(__LINE__, __FILE__, GetLastError());

class Window 
{

//Window exception
public:
	class WindowException : public MLException
	{
	public:
			Window::WindowException(int line, const char* file, HRESULT hr) noexcept : MLException(line, file), hr{ hr } { }
			const char* what() const noexcept override;
			const char* GetType() const noexcept override { return "Window Exception"; }
			HRESULT GetErrorCode() const noexcept;
			std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};
//-------------------

private:
	//singelton manges registers/cleanup of _window calss
	class WindowClass 
	{
	public:
		static const wchar_t* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass() noexcept;
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const wchar_t* wndClassName = L"Direct3D Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const wchar_t* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const wchar_t* title);
	static std::optional<int> ProssessMessages();
	Graphics& getGfx();
	HWND& getHWND() { return this->hWnd; }
	HWND CreateButton(HWND& hwnd)
	{
		 return CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"OK",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD,  // Styles 
			10,         // x position 
			10,         // y position 
			100,        // Button width
			100,        // Button height
			hwnd,     // Parent window
			NULL,       // No menu.
			(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
			NULL);
	}
private:
	static LRESULT CALLBACK HandelMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandelMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard _keyboard;
	Mouse mouse;
private:
	int width = 0;
	int height = 0;
	HWND hWnd;
	std::unique_ptr<Graphics> pGFX;
};