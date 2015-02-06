#ifndef SG_GUI_WINDOW_SIGNALS_H__
#define SG_GUI_WINDOW_SIGNALS_H__

#include "GuiSignals.h"

namespace sg_gui {

/**
 * Base class for window signals.
 */
class WindowSignal : public GuiSignal { public: typedef GuiSignal parent_type; };

/**
 * Request to go to or leave fullscreen mode.
 */
class WindowFullscreen : public WindowSignal {

public:

	typedef WindowSignal parent_type;

	WindowFullscreen(bool fullscreen_ = true) :
		fullscreen(fullscreen_) {}

	bool fullscreen;
};

/**
 * Request to iconfiy.
 */
class WindowIconify : public WindowSignal { public: typedef WindowSignal parent_type; };

} // namespace sg_gui

#endif // SG_GUI_WINDOW_SIGNALS_H__

