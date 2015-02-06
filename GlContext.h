#ifndef GL_CONTEXT_H__
#define GL_CONTEXT_H__

#include <config.h>

// include platform dependent implementations
#if defined(SYSTEM_LINUX) || defined(SYSTEM_FREEBSD)

	#include <sg_gui/linux/GlxContext.h>

#elif defined(SYSTEM_WINDOWS)

	#include <sg_gui/windows/WglContext.h>

#elif defined(SYSTEM_OSX)

	#include <sg_gui/osx/CocoaContext.h>

#endif

#endif // GL_CONTEXT_H__

