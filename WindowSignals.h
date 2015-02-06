#ifndef GUI_WINDOW_SIGNALS_H__
#define GUI_WINDOW_SIGNALS_H__

#include "GuiSignals.h"

namespace gui {

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

} // namespace gui

#endif // GUI_WINDOW_SIGNALS_H__

