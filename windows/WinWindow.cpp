#include <config.h>
#ifdef SYSTEM_WINDOWS

#include <sg_gui/OpenGl.h>
#include <sg_gui/windows/WinWindow.h>
#include <sg_gui/Modifiers.h>
#include <util/Logger.h>

namespace sg_gui {

logger::LogChannel winwinlog("winwinlog", "[WinWindow] ");

WinWindow::WinWindow(std::string caption, const WindowMode& mode) :
	WindowBase(caption) {

}

WinWindow::~WinWindow() {

	LOG_ALL(winwinlog) << "[" << getCaption() << "] destructing..." << std::endl;

	close();

	LOG_ALL(winwinlog) << "[" << getCaption() << "] destructed" << std::endl;
}

void
WinWindow::setFullscreen(bool fullscreen) {

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

	LOG_DEBUG(winwinlog) << "unmapping window " << getCaption() << std::endl;
}

bool
WinWindow::closed(){

	return false;
}

} // namespace sg_gui

#endif
