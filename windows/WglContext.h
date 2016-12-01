#ifndef SG_GUI_GLX_CONTEXT_H__
#define SG_GUI_GLX_CONTEXT_H__

#define NOMINMAX
#include <windows.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <sg_gui/GlContextBase.h>

namespace sg_gui {

// forward declaration
class WinWindow;

class WglContext : public GlContextBase {

public:

	/**
	 * Create an OpenGL context that is not attached to any window.
	 *
	 * @param share A WglContext to share display lists with.
	 */
	WglContext(const ContextSettings& settings, WglContext* share = 0);

	/**
	 * Create an OpenGL context that renders into the specified window.
	 *
	 * @param window The owner of this context.
	 * @param settings The desired settings of the context.
	 * @param share A WglContext to share display lists with.
	 */
	WglContext(WinWindow* window, const ContextSettings& settings, WglContext* shared = 0);

	/**
	 * Release this context.
	 */
	virtual ~WglContext();

	/**
	 * Make this context the active context of the calling thread. If called
	 * with <code>false</code> as argument, deactivates this context (if it was
	 * active) and makes the offline context active for the calling thread.
	 */
	bool activate(bool active = true) override;

	/**
	 * Report whether this context is currently active.
	 *
	 * @return True, if this context is active.
	 */
	bool isActive() override;

	/**
	 * Make visible whatever was rendered using this context.
	 */
	void flush() override;

private:

	/**
	 * Create a context for the current window and display.
	 */
	void createContext(WglContext* share);

	/**
	 * Enables vertical sync if desired by ContextSettings.
	 */
	void enableVerticalSync(bool enable);

	// the window this context renders to
	HWND _window;

	// the device context of the window
	HDC _hdc;

	// have we created the window?
	bool _ownWindow;

	// the true OpenGl context
	HGLRC _context;

	// the settings according to this context
	ContextSettings _settings;

	// indicates that this context is currently active
	bool       _active;
};

} // namespace sg_gui

#endif // SG_GUI_GLX_CONTEXT_H__

