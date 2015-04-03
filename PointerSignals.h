#ifndef SG_GUI_POINTER_SIGNAL_H__
#define SG_GUI_POINTER_SIGNAL_H__

#include "GuiSignals.h"
#include "Modifiers.h"
#include "Buttons.h"

namespace sg_gui {

/**
 * Base class for all 2D input signals (mouse, pen, touch, ...).
 */
class PointerSignal : public InputSignal {

public:

	typedef InputSignal parent_type;

	PointerSignal(
			unsigned long                timestamp,
			const util::point<double,2>& position_,
			const Modifiers&             modifiers_) :
		InputSignal(timestamp),
		position(position_),
		modifiers(modifiers_) {}

	util::point<double,2> position;

	Modifiers modifiers;
};

class PointerMove : public PointerSignal {

public:

	typedef PointerSignal parent_type;

	PointerMove(
			unsigned long                timestamp,
			const util::point<double,2>& position,
			const Modifiers&             modifiers) :
		PointerSignal(timestamp, position, modifiers) {}
};

class PointerDown : public PointerSignal {

public:

	typedef PointerSignal parent_type;

	PointerDown(
			unsigned long                timestamp,
			const util::point<double,2>& position,
			const Modifiers&             modifiers,
			const buttons::Button&       button_) :
		PointerSignal(timestamp, position, modifiers),
		button(button_) {}

	buttons::Button button;
};

class PointerUp : public PointerSignal {

public:

	typedef PointerSignal parent_type;

	PointerUp(
			unsigned long                timestamp,
			const util::point<double,2>& position,
			const Modifiers&             modifiers,
			const buttons::Button&       button_) :
		PointerSignal(timestamp, position, modifiers),
		button(button_) {}

	buttons::Button button;
};

} // namespace sg_gui

#endif // SG_GUI_POINTER_SIGNAL_H__

