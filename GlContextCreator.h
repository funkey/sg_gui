#ifndef SG_GUI_GL_CONTEXT_CREATOR_H__
#define SG_GUI_GL_CONTEXT_CREATOR_H__

namespace sg_gui {

// forward declaration
class GlContext;

/**
 * Interface for classes that are able to create GlContexts.
 */
class GlContextCreator {

public:

	virtual GlContext* createGlContext() = 0;
};

} // namespace sg_gui

#endif // SG_GUI_GL_CONTEXT_CREATOR_H__

