#include <config.h>
#ifdef SYSTEM_WINDOWS

#include "WindowClass.h"

#include <sg_gui/windows/WinWindow.h>
#include <util/Logger.h>

logger::LogChannel windowclasslog("windowclasslog", "[WindowClass] ");

namespace sg_gui {

bool WindowClass::_dummyWindowClassRegistered = false;
bool WindowClass::_defaultWindowClassRegistered = false;

char*
	WindowClass::getDummyWindowClass() {

	if (_dummyWindowClassRegistered)
		return TEXT("dummy");

	WNDCLASS dummyClass;
	dummyClass.style = 0;
	dummyClass.lpfnWndProc = DefWindowProc;
	dummyClass.cbClsExtra = 0;
	dummyClass.cbWndExtra = 0;
	dummyClass.hInstance = GetModuleHandle(NULL);
	dummyClass.hIcon = NULL;
	dummyClass.hCursor = LoadCursor(0, IDC_ARROW);
	dummyClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	dummyClass.lpszMenuName = NULL;
	dummyClass.lpszClassName = TEXT("dummy");

	if (!RegisterClass(&dummyClass))
		LOG_ERROR(windowclasslog) << "could not register window class, error code " << GetLastError() << std::endl;
	else
		_dummyWindowClassRegistered = true;

	return TEXT("dummy");
}

char*
	WindowClass::getDefaultWindowClass() {

	if (_defaultWindowClassRegistered)
		return TEXT("window");

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

	if (!RegisterClass(&windowClass))
		LOG_ERROR(windowclasslog) << "could not register window class, error code " << GetLastError() << std::endl;
	else
		_defaultWindowClassRegistered = true;

	return TEXT("window");
}

} // namespace sg_gui

#endif
