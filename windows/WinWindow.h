#ifndef SG_GUI_X_WINDOW_H__
#define SG_GUI_X_WINDOW_H__

#include <map>
#include <string>

#define NOMINMAX
#include <windows.h>

#include <sg_gui/WindowBase.h>
#include <sg_gui/WindowMode.h>

namespace sg_gui {

/**
 * Linux dependent implementation of the abstract class WindowBase.
 */
class WinWindow : public WindowBase {

public:

	/**
	 * Default constructor.
	 *
	 * @param caption The caption of the window to be created.
	 * @param mode The requested mode of the window (size, position, ...)
	 */
	WinWindow(std::string caption, const WindowMode& mode = WindowMode());

	/**
	 * Desctructor. Closes the window, if still open.
	 */
	~WinWindow();

	/**
	 * Close this window.
	 */
	void close() override;

	/**
	 * Check whether the window was closed by the user.
	 *
	 * @return <code>true</code> if the window was closed.
	 */
	bool closed() override;

	/**
	 * Maximizes the window without decorations.
	 */
	void setFullscreen(bool fullscreen) override;

	/**
	 * Process all accumulated X events since the last call to this function.
	 * This method should be called repeatedly to ensure proper redrawing and
	 * user interaction.
	 */
	void processEvents() override;

	HWND getWindowHandle() const { return _windowHandle; }

private:

	/**
	 * Create a GlContext that is shared with the given global context.
	 */
	WglContext* createSharedGlContext(
			const ContextSettings& settings,
			GlContext*             globalContext) override;

	/**
	 * Static WndProc callback for window class, forwards to non-static windowProc.
	 */
	static LRESULT CALLBACK staticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/**
	 * The message callback.
	 */
	LRESULT CALLBACK windowProc(UINT msg, WPARAM wParam, LPARAM lParam);

	HWND _windowHandle;

	bool _closed;

	bool _fullscreen;
};

} // namespace sg_gui

#endif // SG_GUI_X_WINDOW_H__

