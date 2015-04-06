#include <util/Logger.h>
#include "DisplayConnection.h"

namespace sg_gui {

DisplayConnection DisplayConnection::_instance;

Display*
DisplayConnection::getDisplay() {

	return _instance.__getDisplay();
}

DisplayConnection::DisplayConnection() :
	_display(0) {}

Display*
DisplayConnection::__getDisplay() {

	if (_display == 0)
		_display = XOpenDisplay(0);

	return _display;
}

DisplayConnection::~DisplayConnection() {

	if (_display != 0)
		XCloseDisplay(_display);
}

} // namespace sg_gui
