#include <config.h>
#ifdef SYSTEM_WINDOWS

#include <sg_gui/OpenGl.h>
#include <sg_gui/windows/WinWindow.h>
#include <sg_gui/Modifiers.h>
#include <util/Logger.h>

namespace sg_gui {

logger::LogChannel winwinlog("winwinlog", "[WinWindow] ");

WinWindow::WinWindow(std::string caption, const WindowMode& mode) :
	WindowBase(caption),
	_closed(false),
	_fullscreen(false) {

	WNDCLASS windowClass;
	windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &WinWindow::staticWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = sizeof(WinWindow*); // user data
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = TEXT("window");

	if (!RegisterClass(&windowClass)) {

		LOG_ERROR(winwinlog) << "could not register window class, error code " << GetLastError() << std::endl;
		return;
	}

	int left, top, width, height;
	left = top = width = height = CW_USEDEFAULT;
	if (mode.fullscreen) {

		HMONITOR monitor = MonitorFromWindow(0, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		if (!GetMonitorInfo(monitor, &mi)) {

			LOG_ERROR(winwinlog) << "could not get monitor info" << std::endl;
			return;
		}

		left   = mi.rcMonitor.left;
		top    = mi.rcMonitor.top;
		width  = mi.rcMonitor.right - mi.rcMonitor.left;
		height = mi.rcMonitor.bottom - mi.rcMonitor.top;
	}

	_windowHandle = CreateWindow(
		TEXT("window"),
		TEXT(caption.c_str()),
		WS_OVERLAPPEDWINDOW,
		left, top, width, height,
		NULL, // No parent window
		NULL, // No window menu
		GetModuleHandleW(NULL),
		this); // sneak WinWindow pointer to event callback

	if (!_windowHandle) {

		LOG_ERROR(winwinlog) << "could not create window, error code " << GetLastError() << std::endl;
		return;
	}

	ShowWindow(_windowHandle, SW_SHOW);
	UpdateWindow(_windowHandle);
}

WinWindow::~WinWindow() {

	LOG_DEBUG(winwinlog) << "[" << getCaption() << "] destructing..." << std::endl;

	close();

	LOG_DEBUG(winwinlog) << "[" << getCaption() << "] destructed" << std::endl;
}

void
WinWindow::setFullscreen(bool fullscreen) {

	// TODO: switch forth and back
}

void
WinWindow::processEvents() {

	MSG msg = {};

	int ret;
	while (!closed() && (ret = GetMessage(&msg, _windowHandle, 0, 0)) != 0) {

		if (ret < 0) {

			LOG_ERROR(winwinlog) << "error retrieving message" << std::endl;
			break;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		redraw();
	}
}

WglContext*
WinWindow::createSharedGlContext(const ContextSettings& settings, WglContext* globalContext) {

	return new GlContext(this, settings, globalContext);
}

LRESULT CALLBACK
WinWindow::staticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if (msg == WM_NCCREATE) {

		// get the WinWindow pointer issued via CreateWindow
		LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		void* lpCreateParam = createStruct->lpCreateParams;
		WinWindow* thisWindow = reinterpret_cast<WinWindow*>(lpCreateParam);

		// at this time, _windowHandle should still be zero
		if (thisWindow->_windowHandle != 0) {

			LOG_ERROR(winwinlog) << "initial window handle is not zero" << std::endl;
			return 0;
		}

		// register the WinWindow pointer with this HWND
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisWindow));

		// forward to default handler
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	// get the WinWindow pointer
	WinWindow* thisWindow = reinterpret_cast<WinWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	// WM_NCCREATE wasn't encountered, yet
	if (!thisWindow)
		return DefWindowProc(hwnd, msg, wParam, lParam);

	// CreateWindow didn't return, yet
	if (thisWindow->_windowHandle != hwnd)
		return DefWindowProc(hwnd, msg, wParam, lParam);

	assert(thisWindow->_windowHandle == hwnd);

	// call the member windowProc
	return thisWindow->windowProc(msg, wParam, lParam);
}

LRESULT CALLBACK
WinWindow::windowProc(UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {

	case WM_DESTROY:

		LOG_DEBUG(winwinlog) << "got WM_DESTROY" << std::endl;
		PostQuitMessage(0);
		processCloseEvent();
		break;

	case WM_PAINT:

		LOG_DEBUG(winwinlog) << "got WM_PAINT" << std::endl;
		processExposeEvent();
		// we take care of redrawing, tell windows everything is good
		ValidateRect(_windowHandle, NULL);
		break;

	case WM_SIZE:

	{
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);

		LOG_DEBUG(winwinlog) << "got WM_SIZE with " << width << ", " << height << std::endl;
		processResizeEvent(width, height);
		break;
	}

	default:

		LOG_ALL(winwinlog) << "got message " << msg << std::endl;
		return DefWindowProc(_windowHandle, msg, wParam, lParam);
	}

	return 0;
}

void
WinWindow::close() {

	if (closed())
		return;

	_closed = true;

	LOG_DEBUG(winwinlog) << "unmapping window " << getCaption() << std::endl;
	DestroyWindow(_windowHandle);
	_windowHandle = 0;
}

bool
WinWindow::closed(){

	return _closed;
}

} // namespace sg_gui

#endif
