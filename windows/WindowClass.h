#ifndef SG_GUI_WINDOWS_WINDOW_CLASS_H__
#define SG_GUI_WINDOWS_WINDOW_CLASS_H__

#define NOMINMAX
#include <windows.h>

namespace sg_gui {

class WindowClass {

public:

	static char* getDummyWindowClass();

	static char* getDefaultWindowClass();

private:

	static bool _dummyWindowClassRegistered;

	static bool _defaultWindowClassRegistered;
};

} // namespace sg_gui

#endif // SG_GUI_WINDOWS_WINDOW_CLASS_H__
