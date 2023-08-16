#include "Window.h"
using namespace std;

//Window class stuff
Window::WindowClass Window::WindowClass::wndClass;


Window::WindowClass::WindowClass() noexcept : hInst(GetModuleHandle(nullptr)) 
{

	WNDCLASSEX wc{ 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandelMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32,32,0));
	wc.hCursor = nullptr;//static_cast<HCURSOR>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 16, 16, 0));
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
	RegisterClassEx(&wc);

}

Window::WindowClass::~WindowClass() 
{
	UnregisterClass(wndClassName, GetInstance());
}

const wchar_t* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept 
{
	return wndClass.hInst;
}

//Window stuff
Window::Window(int width, int height, const wchar_t* name) : width(width), height(height)
{
	RECT wr{ 0 };
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (!AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)) {
		throw WIN_EXEPT_LAST();
	}
	//create _window & get hWnd
	hWnd = CreateWindow(WindowClass::GetName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr,
		nullptr,
		WindowClass::GetInstance(),
		this);
	
	if (hWnd == nullptr)
		throw WIN_EXEPT_LAST();
	
	//show _window
	ShowWindow(hWnd, SW_SHOWDEFAULT);

	pGFX = std::make_unique<Graphics>(hWnd);
	
}

Window::~Window() 
{
	DestroyWindow(hWnd);
}

void Window::SetTitle(const wchar_t* title)
{
	SetWindowText(hWnd, title);
}

std::optional<int> Window::ProssessMessages()
{
	MSG msg;

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) 
	{
		if (msg.message == WM_QUIT) 
		{
			return msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

Graphics& Window::getGfx()
{
	return *pGFX;
}

LRESULT CALLBACK Window::HandelMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//use create parameter passed in from CreateWindow() to store _window class pointer
	if (msg == WM_NCCREATE) 
	{
		//extract ptr to _window class from create data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		//set WinAPI-managed user data to store ptr to _window class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		//set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandelMsgThunk));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
}

LRESULT CALLBACK Window::HandelMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	//retrieve ptr to _window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	//forward message to _window class handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	POINTS pt;
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_KILLFOCUS:
		_keyboard.ClearState();
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000) ||_keyboard.AutorepeatIsEnabled()) 
		{
			_keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		_keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		_keyboard.OnChar(static_cast<unsigned char>(wParam));
		break;
	case WM_MOUSEMOVE:
		pt = MAKEPOINTS(lParam);
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height) 
		{
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow()) 
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		else 
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON)) 
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			else 
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	case WM_LBUTTONDOWN:
		pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	case WM_RBUTTONDOWN:
		pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	case WM_LBUTTONUP:
		pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	case WM_RBUTTONUP:
		pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	case WM_MOUSEWHEEL:
		pt = MAKEPOINTS(lParam);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) 
		{
			mouse.OnWheelUp(pt.x, pt.y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) 
		{
			mouse.OnWheelDown(pt.x, pt.y);
		}
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//Window exception
const char* Window::WindowException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << "\n"
		<< "[ERROR CODE] " << GetErrorCode() << "\n"
		<< "[DESCRIPTION] " << GetErrorString() << "\n"
		<< GetOriginString();
	this->whatBuffer = oss.str();
	return whatBuffer.c_str();
}

HRESULT Window::WindowException::GetErrorCode() const noexcept 
{
	return this->hr;
}

std::string Window::WindowException::GetErrorString() const noexcept
{
	return std::system_category().message(this->hr);
}