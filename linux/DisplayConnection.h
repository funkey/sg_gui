#ifndef SG_GUI_DISPLAY_H__
#define SG_GUI_DISPLAY_H__

#include <X11/Xlib.h>

namespace sg_gui {

/**
 * Singleton class to hold the X display connection.
 */
class DisplayConnection {

public:

	static ::Display* getDisplay();

private:

	DisplayConnection();

	~DisplayConnection();

	::Display* __getDisplay();

	static DisplayConnection _instance;

	Display* _display;
};

} // namespace sg_gui

#endif // SG_GUI_DISPLAY_H__

