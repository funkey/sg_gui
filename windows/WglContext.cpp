#include <config.h>
#ifdef SYSTEM_WINDOWS

#include <util/Logger.h>

#include <sg_gui/windows/WglContext.h>
#include <sg_gui/windows/WindowClass.h>
#include <sg_gui/OpenGl.h>
#include <sg_gui/Window.h>

using namespace logger;

LogChannel wgllog("wgllog", "[WglContext] ");

namespace sg_gui {

WglContext::WglContext(const ContextSettings& settings, WglContext* share) :
	_hdc(0),
	_ownWindow(true),
	_context(0),
	_settings(settings),
	_active(false) {

	_window = CreateWindow(
		WindowClass::getDummyWindowClass(),
		TEXT("dummy"),
		WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, // No parent window
		NULL, // No window menu
		GetModuleHandleW(NULL),
		NULL);

	if (!_window) {

		LOG_ERROR(wgllog) << "could not create dummy window, error code " << GetLastError() << std::endl;
		return;
	}

	_hdc = GetDC(_window);
	createContext(share);
}

WglContext::WglContext(WinWindow* window, const ContextSettings& settings, WglContext* share) :
	_window(window->getWindowHandle()),
	_hdc(0),
	_ownWindow(false),
	_context(0),
	_settings(settings),
	_active(false) {

	if (!_window) {

		LOG_ERROR(wgllog) << "passed window is not valid" << std::endl;
		return;
	}

	_hdc = GetDC(_window);
	createContext(share);
}

WglContext::~WglContext() {

	if (_context) {

		if (wglGetCurrentContext() == _context)
			wglMakeCurrent(0, 0);

		wglDeleteContext(_context);
	}

	if (_window && _ownWindow)
		DestroyWindow(_window);
}

bool
WglContext::activate(bool active) {

	if (active) {

		if (!_context) {

			LOG_ERROR(wgllog) << "attempted to activate an "
			                  << "uninitialised context" << std::endl;

			_active = false;

			return false;
		}

		if (!wglMakeCurrent(_hdc, _context)) {

			LOG_ERROR(wgllog) << "failed to make context current"
			                  << std::endl;

			_active = false;

			return false;
		}

		_active = true;

		return true;

	} else {

		_active = false;

		return (wglMakeCurrent(0, 0) != 0);
	}
}

bool
WglContext::isActive() {

	return _active;
}

void
WglContext::flush() {

	if (_hdc)
		SwapBuffers(_hdc);
}

void
WglContext::enableVerticalSync(bool enable) {

	// TODO
}

void
WglContext::createContext(WglContext* share) {

	// TODO: handle _settings
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,
		8,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int pixelFormat = ChoosePixelFormat(_hdc, &pfd);
	if (!pixelFormat) {

		LOG_ERROR(wgllog) << "could not select a pixel format, error code " << GetLastError() << std::endl;
		return;
	}

	SetPixelFormat(_hdc, pixelFormat, &pfd);

	_context = wglCreateContext(_hdc);
}


} // namespace sg_gui

#endif
