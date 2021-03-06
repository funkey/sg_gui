#ifndef SG_GUI_X_WINDOW_H__
#define SG_GUI_X_WINDOW_H__

#include <map>
#include <string>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

#include <sg_gui/WindowBase.h>
#include <sg_gui/WindowMode.h>

namespace sg_gui {

/**
 * Linux dependent implementation of the abstract class WindowBase.
 */
class XWindow : public WindowBase {

public:

	/**
	 * Default constructor.
	 *
	 * @param caption The caption of the window to be created.
	 * @param mode The requested mode of the window (size, position, ...)
	 */
	XWindow(std::string caption, const WindowMode& mode = WindowMode());

	/**
	 * Desctructor. Closes the window, if still open.
	 */
	~XWindow();

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

	/**
	 * Provides access to the X11 display to create an OpenGl context.
	 */
	Display* getDisplay() { return _display; }

	/**
	 * Provides access to the X11 window to create an OpenGl context.
	 */
	::Window getX11Window() { return _window; }

private:

	/**
	 * Classification of input devices.
	 */
	enum InputType {

		Mouse,
		Pen,
		Touch,
		Ignored /* for input devices we don't care about */
	};

	/**
	 * Enums to add, remove, or toggle window manager properties.
	 */
	enum {

		_NET_WM_STATE_REMOVE = 0,
		_NET_WM_STATE_ADD = 1,
		_NET_WM_STATE_TOGGLE = 2
	};

	/**
	 * Create a GlContext that is shared with the given global context.
	 */
	GlxContext* createSharedGlContext(
			const ContextSettings& settings,
			GlContext*             globalContext) override;

	/**
	 * Interrupt the call to waitForEvents().
	 */
	void interrupt() override;

	/**
	 * Process a general X event.
	 */
	void processEvent(XEvent& event);

	/**
	 * Wait for X events. Returns true if there are events, false if the wait 
	 * got interrupted.
	 */
	bool waitForEvents();

	/**
	 * Process a xinput2 property change event.
	 */
	void processPropertyEvent(XIPropertyEvent* propertyEvent);

	void configureTabletArea(int deviceId);

	void processPenStatusEvent(XIPropertyEvent* propertyEvent);

	/**
	 * Converts an X keycode to a Key.
	 */
	keys::Key keycodeToKey(unsigned int keycode);

	/**
	 * Converts an X state to Modifiers.
	 */
	Modifiers stateToModifiers(unsigned int state);

	/**
	 * Converts an X button to Modifiers.
	 */
	Modifiers buttonsToModifiers(XIButtonState& buttons);

	/**
	 * Converts an X button to a Button.
	 */
	buttons::Button buttonToButton(unsigned int xbutton);

	/**
	 * Determine the input type given a device id.
	 */
	InputType getInputType(int deviceid);

	/**
	 * Transform the pen device coordinates to screen coordinates according to 
	 * the calibration.
	 */
	util::point<double,2> getPenPosition(XIDeviceEvent* event);

	/**
	 * Assuming the input device is a pen, get the pressure level.
	 */
	double getPressure(XIDeviceEvent* event);

	// the X11 display
	Display* _display;

	// the X11 screen
	int      _screen;

	// file descriptor to X11 events
	int      _xfd;

	// the X11 window
	::Window _window;

	// the X11 atom for delete window events
	Atom     _deleteWindow;

	// the X11 input method
	XIM      _inputMethod;

	// the X11 input context
	XIC      _inputContext;

	// the opcode for xinput2 events
	int      _xinputOpcode;

	// was closed
	bool     _closed;

	// pipe file descriptors to interrupt the event thread
	int  _interruptFds[2];

	// is this window running in fullscreen?
	bool     _fullscreen;

	// the current resolution of the screen
	util::point<int,2> _screenResolution;

	// map from device ID to input type
	std::map<int, InputType> _inputTypes;

	// list of input devices of type pen
	std::vector<int> _penDevices;

	// calibration for the pen
	double _penSlopeX;
	double _penSlopeY;
	double _penOffsetX;
	double _penOffsetY;

	// the X11 atom for the wacom id property
	Atom _serialIdsProperty;

	// the X11 atom for the wacom tablet area
	Atom _tabletAreaProperty;
};

} // namespace sg_gui

#endif // SG_GUI_X_WINDOW_H__

