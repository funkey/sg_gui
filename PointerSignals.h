#ifndef SG_GUI_POINTER_SIGNAL_H__
#define SG_GUI_POINTER_SIGNAL_H__

#include "GuiSignals.h"
#include "Modifiers.h"

namespace sg_gui {

/**
 * Base class for all 2D input signals (mouse, pen, touch, ...).
 */
class PointerSignal : public InputSignal {

public:

	typedef InputSignal parent_type;

	PointerSignal(
			unsigned long              timestamp,
			const util::point<double>& position_,
			const Modifiers&           modifiers_) :
		InputSignal(timestamp),
		position(position_),
		modifiers(modifiers_) {}

	util::point<double> position;

	Modifiers modifiers;
};

class PointerMove : public PointerSignal {

public:

	typedef PointerSignal parent_type;

	PointerMove(
			unsigned long              timestamp,
			const util::point<double>& position,
			const Modifiers&           modifiers) :
		PointerSignal(timestamp, position, modifiers) {}
};

class PointerDown : public PointerSignal {

public:

	typedef PointerSignal parent_type;

	PointerDown(
			unsigned long              timestamp,
			const util::point<double>& position,
			const Modifiers&           modifiers) :
		PointerSignal(timestamp, position, modifiers) {}
};

class PointerUp : public PointerSignal {

public:

	typedef PointerSignal parent_type;

	PointerUp(
			unsigned long              timestamp,
			const util::point<double>& position,
			const Modifiers&           modifiers) :
		PointerSignal(timestamp, position, modifiers) {}
};

} // namespace sg_gui

#endif // SG_GUI_POINTER_SIGNAL_H__

