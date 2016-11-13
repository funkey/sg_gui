#ifndef SG_GUI_WINDOW_H__
#define SG_GUI_WINDOW_H__

#include <config.h>

#if defined(SYSTEM_UNIX)
#include <sg_gui/linux/XWindow.h>
#elif defined(SYSTEM_WINDOWS)
#include <sg_gui/windows/WinWindow.h>
#elif defined(SYSTEM_OSX)
#include <sg_gui/osx/CocoaWindow.h>
#endif

namespace sg_gui {

#if defined(SYSTEM_UNIX)

	typedef XWindow Window;

#elif defined(SYSTEM_WINDOWS)

	typedef WinWindow Window;

#elif defined(SYSTEM_OSX)

	typedef CocoaWindow Window;

#endif

}

#endif // SG_GUI_WINDOW_H__
