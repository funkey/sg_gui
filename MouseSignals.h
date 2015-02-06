#ifndef GUI_MOUSE_SIGNALS_H__
#define GUI_MOUSE_SIGNALS_H__

#include "PointerSignal.h"
#include <sg_gui/Buttons.h>
#include <sg_gui/Modifiers.h>

namespace gui {

class MouseSignal : public PointerSignal {

public:

	typedef PointerSignal parent_type;

	MouseSignal() :
		modifiers(NoModifier) {}

	MouseSignal(
			unsigned long timestamp,
			const util::point<double>& position_,
			const Modifiers& modifiers_) :
		PointerSignal(timestamp, position_),
		modifiers(modifiers_) {}

	Modifiers modifiers;
};

class MouseMove : public MouseSignal {

public:

	typedef MouseSignal parent_type;

	MouseMove() {}

	MouseMove(
			unsigned long timestamp,
			const util::point<double>& position,
			const Modifiers& modifiers) :
		MouseSignal(timestamp, position, modifiers) {}
};

class MouseDown : public MouseSignal {

public:

	typedef MouseSignal parent_type;

	MouseDown() :
		button(buttons::NoButton) {}

	MouseDown(
			unsigned long timestamp,
			const buttons::Button& button_,
			const util::point<double>& position,
			const Modifiers& modifiers) :
		MouseSignal(timestamp, position, modifiers),
		button(button_) {}

	buttons::Button button;
};

class MouseUp   : public MouseSignal {

public:

	typedef MouseSignal parent_type;

	MouseUp() {}

	MouseUp(
			unsigned long timestamp,
			const buttons::Button& button_,
			const util::point<double>& position,
			const Modifiers& modifiers) :
		MouseSignal(timestamp, position, modifiers),
		button(button_) {}

	buttons::Button button;
};

} // namespace gui

#endif // GUI_MOUSE_SIGNALS_H__

