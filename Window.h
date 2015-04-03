#ifndef SG_GUI_WINDOW_H__
#define SG_GUI_WINDOW_H__

#include <string>

#include <scopegraph/Scope.h>
#include <sg_gui/WindowMode.h>
#include <sg_gui/error_handling.h>
#include <sg_gui/GlContext.h>
#include <sg_gui/GlContextCreator.h>
#include <sg_gui/FingerSignals.h>
#include <sg_gui/PenSignals.h>
#include <sg_gui/MouseSignals.h>
#include <sg_gui/KeySignals.h>
#include <sg_gui/WindowSignals.h>
#include <sg_gui/Painter.h>
#include <config.h>

// include platform dependent implementations
#if defined(SYSTEM_LINUX) || defined(SYSTEM_FREEBSD)

	#include <sg_gui/linux/XWindow.h>
	typedef sg_gui::XWindow WindowType;

#elif defined(SYSTEM_WINDOWS)

	#include <sg_gui/windows/WinWindow.h>
	typedef sg_gui::WinWindow WindowType;

#elif defined(SYSTEM_OSX)

	#include <sg_gui/osx/CocoaWindow.h>
	typedef sg_gui::CocoaWindow WindowType;

#endif

namespace sg_gui {

using namespace util;

/**
 * Platform independent window class. Allows views to be attached to it to draw
 * OpenGL content.
 */
class Window :
		public sg::Scope<
				Window,
				sg::ProvidesInner<
						DrawOpaque,
						DrawTranslucent,
						Resize,
						KeyDown,
						KeyUp,
						FingerMove,
						FingerDown,
						FingerUp,
						PenMove,
						PenDown,
						PenUp,
						PenIn,
						PenOut,
						PenAway,
						MouseMove,
						MouseDown,
						MouseUp
				>,
				sg::AcceptsInner<
						ContentChanged,
						SizeChanged,
						WindowFullscreen
				>
		>,
		public WindowType,
		public GlContextCreator {

public:

	/**
	 * Creates a window with the specified mode.
	 *
	 * @param caption The caption of the window to be created.
	 * @param mode The requested mode of the window (size, position, ...)
	 */
	Window(std::string caption, const WindowMode& mode = WindowMode());

	virtual ~Window();

	/**
	 * Get the resolution of this window.
	 *
	 * @return The resolution of this window.
	 */
	const point<double,2>& getResolution();

	/**
	 * Set the background color of this window.
	 *
	 * @param r The red portion in [0,1].
	 * @param g The red portion in [0,1].
	 * @param b The red portion in [0,1].
	 */
	void setBackgroundColor(double r, double g, double b) {
	    _clear_r = r;
	    _clear_g = g;
	    _clear_b = b;
	}

	/**
	 * If called, the outcome of the next redraw will be saved in a png file.
	 */
	void requestFrameSave();

	/**
	 * Create a GlContext that allows drawing to this window.
	 */
	GlContext* createGlContext();

	/**
	 * Pipeline callback for size change signals from the painter.
	 */
	void onInnerSignal(SizeChanged& signal);

	/**
	 * Pipeline callback for content change signals from the painter.
	 */
	void onInnerSignal(ContentChanged& signal);

	/**
	 * Pipeline callback for fullscreen requests.
	 */
	void onInnerSignal(WindowFullscreen& signal);

private:

	using WindowType::setDirty;

	/**
	 * Overwritten from SimpleProcessNode. We are a sink, there's nothing to do
	 * here.
	 */
	void updateOutputs() {};

	/**
	 * Callback for top-down resize events.
	 *
	 * @return True, if the size of the window changed and a redraw is in order.
	 */
	bool processResizeEvent(int width, int height);

	/**
	 * Callback for close events.
	 */
	void processCloseEvent();

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processKeyUpEvent(const keys::Key& key, const Modifiers& modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processKeyDownEvent(const keys::Key& key, const Modifiers& modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processFingerUpEvent(
			unsigned long                timestamp,
			const util::point<double,2>& position,
			int                          id,
			const Modifiers&             modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processFingerDownEvent(
			unsigned long                timestamp,
			const util::point<double,2>& position,
			int                          id,
			const Modifiers&             modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processFingerMoveEvent(
			unsigned long                timestamp,
			const util::point<double,2>& position,
			int                          id,
			const Modifiers&             modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processPenUpEvent(
			unsigned long                timestamp,
			const buttons::Button&       button,
			const util::point<double,2>& position,
			double                       pressure,
			const Modifiers&             modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processPenDownEvent(
			unsigned long                timestamp,
			const buttons::Button&       button,
			const util::point<double,2>& position,
			double                       pressure,
			const Modifiers&             modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processPenMoveEvent(
			unsigned long                timestamp,
			const util::point<double,2>& position,
			double                       pressure,
			const Modifiers&             modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processPenInEvent(
			unsigned long              timestamp);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processPenOutEvent(
			unsigned long              timestamp);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processPenAwayEvent(
			unsigned long              timestamp);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processButtonUpEvent(
			unsigned long                timestamp,
			const buttons::Button&       button,
			const util::point<double,2>& position,
			const Modifiers&             modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processButtonDownEvent(
			unsigned long                timestamp,
			const buttons::Button&       button,
			const util::point<double,2>& position,
			const Modifiers&             modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processMouseMoveEvent(
			unsigned long                timestamp,
			const util::point<double,2>& position,
			const Modifiers&             modifiers);

	/**
	 * Redraw the content of the window.
	 */
	void redraw();

	/**
	 * Allocate memory for the frame buffer.
	 */
	void createFrameBuffer();

	/**
	 * Free memory of the frame buffer.
	 */
	void deleteFrameBuffer();

	/**
	 * Clear the window with the background color.
	 */
	void clear();

	/**
	 * Show the updates from the views.
	 */
	void flush();

	/**
	 * Reconfigure the OpenGL viewport on window resize events.
	 */
	void configureViewport();

	/**
	 * Save the current content of the window to a file.
	 */
	void saveFrame();

	// the region displayed by this window in GL units
	rect<double>  _region;

	// the resolution of the region in pixels
	point<double,2> _resolution;

	// set to true if the next frame should be saved to file
	bool          _saveFrameRequest;

#ifdef HAVE_PNG
	// the next frame number
	unsigned      _frameNumber;
#endif

	// a buffer to save the frame to file (exclusively owned)
	unsigned char* _frameBuffer;

	// the background color of this window
	double        _clear_r, _clear_g, _clear_b;
};

} // namespace sg_gui

#endif // SG_GUI_WINDOW_H__

