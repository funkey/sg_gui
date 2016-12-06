#ifndef SG_GUI_WINDOW_BASE_H__
#define SG_GUI_WINDOW_BASE_H__

#include <string>

#include <scopegraph/Scope.h>
#include <sg_gui/GlContext.h>
#include <sg_gui/GlContextCreator.h>
#include <sg_gui/FingerSignals.h>
#include <sg_gui/PenSignals.h>
#include <sg_gui/MouseSignals.h>
#include <sg_gui/KeySignals.h>
#include <sg_gui/WindowSignals.h>
#include <sg_gui/Keys.h>
#include <sg_gui/Buttons.h>
#include <sg_gui/Modifiers.h>
#include <util/box.hpp>
#include <util/point.hpp>
#include <config.h>

namespace sg_gui {

/**
 * Base class providing common functionality for all window
 * implementations.
 *
 * Subclasses are expected to implement all pure virtual functions (obviously) 
 * and create (and show) the window in the constructor.
 */
class WindowBase :
		public sg::Scope<
				WindowBase,
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
		public GlContextCreator {

public:

	WindowBase(std::string caption);

	virtual ~WindowBase();

	/**
	 * Close this window and release all resources.
	 *
	 * Platform dependent.
	 */
	virtual void close() = 0;

	/**
	 * Check whether the window was closed.
	 *
	 * @return <code>true</code> if the window was closed.
	 *
	 * Platform dependent.
	 */
	virtual bool closed() = 0;

	/**
	 * Change to or from fullscreen mode.
	 *
	 * Platform dependent.
	 */
	virtual void setFullscreen(bool fullscreen) = 0;

	/**
	 * Set the background color of this window.
	 *
	 * @param r The red portion in [0,1].
	 * @param g The red portion in [0,1].
	 * @param b The red portion in [0,1].
	 */
	void setBackgroundColor(float r, float g, float b) {

	    _clear_r = r;
	    _clear_g = g;
	    _clear_b = b;
	}

	/**
	 * Process window manager events and redraw. If this call blocks for events, 
	 * make sure to implement interrupt(), which will be called whenever a 
	 * redraw is required.
	 *
	 * Platform dependent.
	 */
	virtual void processEvents() = 0;

	/**
	 * Create a GlContext that allows drawing to this window.
	 */
	GlContext* createGlContext() override;

	/**
	 * Save the current content of the window to a file.
	 */
	void saveFrame();

	/**
	 * If called, the outcome of the next redraw will be saved in a png file.
	 */
	void requestNextFrameSave();

	/**
	 * Callback for size change signals.
	 */
	void onInnerSignal(SizeChanged& signal);

	/**
	 * Callback for content change signals.
	 */
	void onInnerSignal(ContentChanged& signal);

	/**
	 * Callback for fullscreen requests.
	 */
	void onInnerSignal(WindowFullscreen& signal);

protected:

	/**
	 * Get the caption of this window.
	 */
	std::string getCaption() {

		return _caption;
	}

	/**
	 * Get the resolution of this window.
	 *
	 * @return The resolution of this window.
	 */
	const util::point<float,2>& getResolution() {

		return _resolution;
	}

	/**
	 * Mark this window as being dirty. redraw() will be called on the next
	 * occasion from the event loop in processEvents(). Subclasses should use 
	 * this method to initiate redrawing instead of calling redraw() themselves.
	 *
	 * @param dirty
	 *             Set or unset the dirty flag.
	 *
	 * @param needInterrupt
	 *             Indicate that the event wait needs to be interrupted. Unless 
	 *             you call this from within the event processing code, you want 
	 *             that.
	 */
	inline void setDirty(bool dirty = true, bool needInterrupt = true) {
	
		_dirty = dirty;

		// interrupt the possibly blocking event loop in processEvents()
		if (dirty && needInterrupt)
			interrupt();
	}

	/**
	 * Find out whether this window has been flagged to be dirty. This should be
	 * used in the platform dependent processEvents() to initiate redrawing.
	 */
	bool isDirty() {

		return _dirty;
	}

	/**
	 * Instruct the window to redraw itself, if needed. Should be called from 
	 * the subclass event loop periodically.
	 */
	void redraw();

	/**
	 * React on expose events.
	 *
	 * To be called from processEvents() by subclasses.
	 */
	void processExposeEvent();

	/**
	 * React on resize events. Returns true, if the size was really changed.
	 *
	 * To be called from processEvents() by subclasses.
	 */
	bool processResizeEvent(int width, int height);

	/**
	 * React on close events.
	 *
	 * To be called from processEvents().
	 */
	void processCloseEvent();

	/*******************************************************
	 * Input event callbacks.
	 *
	 * To be called from processEvents() by subclasses.
	 *******************************************************/

	inline void processKeyUpEvent(const keys::Key& key, const Modifiers& modifiers) {

		sendInner<KeyUp>(key, modifiers);
	}

	inline void processKeyDownEvent(const keys::Key& key, const Modifiers& modifiers) {

		sendInner<KeyDown>(key, modifiers);
	}

	inline void processFingerUpEvent(
			unsigned long               timestamp,
			const util::point<float,2>& position,
			int                         id,
			const Modifiers&            modifiers) {

		sendInner<FingerUp>(timestamp, position, id, modifiers);
	}

	inline void processFingerDownEvent(
			unsigned long               timestamp,
			const util::point<float,2>& position,
			int                         id,
			const Modifiers&            modifiers) {

		sendInner<FingerDown>(timestamp, position, id, modifiers);
	}

	inline void processFingerMoveEvent(
			unsigned long               timestamp,
			const util::point<float,2>& position,
			int                         id,
			const Modifiers&            modifiers) {

		sendInner<FingerMove>(timestamp, position, id, modifiers);
	}

	inline void processPenUpEvent(
			unsigned long               timestamp,
			const buttons::Button&      button,
			const util::point<float,2>& position,
			float                       pressure,
			const Modifiers&            modifiers) {

		sendInner<PenUp>(timestamp, button, position, pressure, modifiers);
	}

	inline void processPenDownEvent(
			unsigned long               timestamp,
			const buttons::Button&      button,
			const util::point<float,2>& position,
			float                       pressure,
			const Modifiers&            modifiers) {

		sendInner<PenDown>(timestamp, button, position, pressure, modifiers);
	}

	inline void processPenMoveEvent(
			unsigned long               timestamp,
			const util::point<float,2>& position,
			float                       pressure,
			const Modifiers&            modifiers) {

		sendInner<PenMove>(timestamp, position, pressure, modifiers);
	}

	inline void processPenInEvent(unsigned long timestamp) {

		sendInner<PenIn>(timestamp);
	}

	inline void processPenOutEvent(unsigned long timestamp) {

		sendInner<PenOut>(timestamp);
	}

	inline void processPenAwayEvent(unsigned long timestamp) {

		sendInner<PenAway>(timestamp);
	}

	inline void processButtonUpEvent(
			unsigned long               timestamp,
			const buttons::Button&      button,
			const util::point<float,2>& position,
			const Modifiers&            modifiers) {

		sendInner<MouseUp>(timestamp, button, position, modifiers);
	}

	inline void processButtonDownEvent(
			unsigned long               timestamp,
			const buttons::Button&      button,
			const util::point<float,2>& position,
			const Modifiers&            modifiers) {

		sendInner<MouseDown>(timestamp, button, position, modifiers);
	}

	inline void processMouseMoveEvent(
			unsigned long               timestamp,
			const util::point<float,2>& position,
			const Modifiers&            modifiers) {

		sendInner<MouseMove>(timestamp, position, modifiers);
	}

private:

	/**
	 * Create a GlContext that is shared with the given global context.
	 *
	 * Platform dependent.
	 */
	virtual GlContext* createSharedGlContext(
			const ContextSettings& settings,
			GlContext*             globalContext) = 0;

	/**
	 * Interrupt the event thread. This method will be called whenever a redraw 
	 * is required. Implementations should guarantee that the event loop gets 
	 * interrupted and initiates the redraw.
	 *
	 * Platform dependent.
	 */
	virtual void interrupt() {}

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
	 * Allocate memory for the frame buffer.
	 */
	void createFrameBuffer();

	/**
	 * Free memory of the frame buffer.
	 */
	void deleteFrameBuffer();

	// the name of the window
	std::string _caption;

	// the region displayed by this window in GL units
	util::box<float,2> _region;

	// the resolution of the region in pixels
	util::point<float,2> _resolution;

	// set to true if the next frame should be saved to file
	bool _saveFrameRequest;

#ifdef HAVE_PNG
	// the next frame number
	unsigned _frameNumber;
#endif

	// a buffer to save the frame to file (exclusively owned)
	unsigned char* _frameBuffer;

	// the background color of this window
	float _clear_r, _clear_g, _clear_b;

	bool _dirty;
};

} // namespace sg_gui

#endif // SG_GUI_WINDOW_BASE_H__

