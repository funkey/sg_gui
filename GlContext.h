#ifndef SG_GUI_GL_CONTEXT_H__
#define SG_GUI_GL_CONTEXT_H__

#include <config.h>

#if defined(SYSTEM_UNIX)
#include <sg_gui/linux/GlxContext.h>
#elif defined(SYSTEM_WINDOWS)
#include <sg_gui/windows/WglContext.h>
#elif defined(SYSTEM_OSX)
#include <sg_gui/osx/CocoaContext.h>
#endif

namespace sg_gui {

#if defined(SYSTEM_UNIX)

	typedef GlxContext GlContext;

#elif defined(SYSTEM_WINDOWS)

	typedef WglContext GlContext;

#elif defined(SYSTEM_OSX)

	typedef CocoaContext GlContext;

#endif

}

#endif // SG_GUI_GL_CONTEXT_H__

