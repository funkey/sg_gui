/* Parts of the source code of this file have been taken from the great SFML
 * project: http://www.sfml-dev.org/.
 */

#include <util/Logger.h>

#include <sg_gui/windows/WglContext.h>
#include <sg_gui/OpenGl.h>
#include <sg_gui/Window.h>

using namespace logger;

LogChannel wgllog("wgllog", "[WglContext] ");

namespace sg_gui {

WglContext::WglContext(const ContextSettings& settings, WglContext* share) :
	_ownWindow(true),
	_active(false) {
}

WglContext::WglContext(Window* window, const ContextSettings& settings, WglContext* share) :
	_ownWindow(false) {

}

WglContext::~WglContext() {

}

bool
WglContext::activate(bool active) {

}

bool
WglContext::isActive() {

	return _active;
}

void
WglContext::flush() {

}

void
WglContext::enableVerticalSync(bool enable) {

}

void
WglContext::createContext(const ContextSettings& settings, WglContext* share) {

}

} // namespace sg_gui
