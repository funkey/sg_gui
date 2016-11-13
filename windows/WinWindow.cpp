#include <cstdlib>
#include <fcntl.h>

#include <boost/timer/timer.hpp>

#include <X11/extensions/Xrandr.h>

#include <sg_gui/OpenGl.h>
#include <sg_gui/linux/WinWindow.h>
#include <sg_gui/Modifiers.h>
#include <util/Logger.h>
#include "DisplayConnection.h"

namespace sg_gui {

logger::LogChannel winwinlog("winwinlog", "[WinWindow] ");

WinWindow::WinWindow(std::string caption, const WindowMode& mode) :
	WindowBase(caption),
	_closed(false),
	_fullscreen(false) {

}

WinWindow::~WinWindow() {

	LOG_ALL(winwinlog) << "[" << getCaption() << "] destructing..." << std::endl;

	close();

	LOG_ALL(winwinlog) << "[" << getCaption() << "] destructed" << std::endl;
}

void
WinWindow::setFullscreen(bool fullscreen) {

	_fullscreen = fullscreen;
}

void
WinWindow::processEvents() {

	while (!closed()) {

		//if (waitForEvents()) {

		//}

		redraw();
	}
}

WglContext*
WinWindow::createSharedGlContext(const ContextSettings& settings, WglContext* globalContext) {

	return new GlContext(this, settings, globalContext);
}

void
WinWindow::close() {

	if (closed())
		return;

	_closed = true;

	LOG_DEBUG(winwinlog) << "unmapping window " << getCaption() << std::endl;
}

bool
WinWindow::closed(){

	return _closed;
}

} // namespace sg_gui
