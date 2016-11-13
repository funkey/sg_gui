#ifndef SG_GUI_GL_CONTEXT_CREATOR_H__
#define SG_GUI_GL_CONTEXT_CREATOR_H__

#include "GlContext.h"

namespace sg_gui {

/**
 * Interface for classes that are able to create GlContexts.
 */
class GlContextCreator {

public:

	virtual GlContext* createGlContext() = 0;
};

} // namespace sg_gui

#endif // SG_GUI_GL_CONTEXT_CREATOR_H__

