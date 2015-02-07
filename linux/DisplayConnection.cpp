#include <util/Logger.h>
#include "DisplayConnection.h"

namespace sg_gui {

extern logger::LogChannel xlog;

DisplayConnection DisplayConnection::_instance;

Display*
DisplayConnection::getDisplay() {

	return _instance.__getDisplay();
}

DisplayConnection::DisplayConnection() :
	_display(0) {}

Display*
DisplayConnection::__getDisplay() {

	if (_display == 0) {

		LOG_DEBUG(xlog) << "opening display" << std::endl;
		_display = XOpenDisplay(0);
	}

	return _display;
}

DisplayConnection::~DisplayConnection() {

	LOG_DEBUG(xlog) << "closing display" << std::endl;

	if (_display != 0)
		XCloseDisplay(_display);
}

} // namespace sg_gui
