#ifndef SG_GUI_GL_CONTEXT_BASE_H__
#define SG_GUI_GL_CONTEXT_BASE_H__

#include <cstdlib>

#include <sg_gui/ContextSettings.h>

namespace sg_gui {

class GlContextBase {

public:

	/**
	 * Make this context the active context of the calling thread. If called
	 * with <code>false</code> as argument, deactivates this context (if it was
	 * active).
	 *
	 * Platform dependent.
	 */
	virtual bool activate(bool active = true) = 0;

	/**
	 * Report whether this context is currently active.
	 *
	 * @return True, if this context is active.
	 */
	virtual bool isActive() = 0;

	/**
	 * Make visible whatever was rendered using this context.
	 *
	 * Platform dependent.
	 */
	virtual void flush() = 0;

protected:

	/**
	 * Uses a simple procedure to evalute the score of a given format to the
	 * desired one. The lower, the better.
	 */
	int evaluateFormat(
			const ContextSettings& settings,
			int depthBits,
			int stencilBits,
			int antialiasing) {

		return std::abs(static_cast<int>(settings.depthBits         - depthBits))   +
			   std::abs(static_cast<int>(settings.stencilBits       - stencilBits)) +
			   std::abs(static_cast<int>(settings.antialiasingLevel - antialiasing));
	}
};

} // namespace sg_gui

#endif // SG_GUI_GL_CONTEXT_BASE_H__

