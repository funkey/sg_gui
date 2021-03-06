#ifndef SG_GUI_MOUSE_SIGNALS_H__
#define SG_GUI_MOUSE_SIGNALS_H__

#include "PointerSignals.h"
#include <sg_gui/Modifiers.h>

namespace sg_gui {

class MouseMove : public PointerMove {

public:

	typedef PointerMove parent_type;

	MouseMove(
			unsigned long timestamp,
			const util::point<float,2>& position,
			const Modifiers& modifiers) :
		PointerMove(timestamp, position, modifiers) {}
};

class MouseDown : public PointerDown {

public:

	typedef PointerDown parent_type;

	MouseDown(
			unsigned long timestamp,
			const buttons::Button& button,
			const util::point<float,2>& position,
			const Modifiers& modifiers) :
		PointerDown(timestamp, position, modifiers, button) {}
};

class MouseUp : public PointerUp {

public:

	typedef PointerUp parent_type;

	MouseUp(
			unsigned long timestamp,
			const buttons::Button& button,
			const util::point<float,2>& position,
			const Modifiers& modifiers) :
		PointerUp(timestamp, position, modifiers, button) {}
};

} // namespace sg_gui

#endif // SG_GUI_MOUSE_SIGNALS_H__

