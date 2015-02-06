#ifndef GUI_PEN_SIGNALS_H__
#define GUI_PEN_SIGNALS_H__

#include "PointerSignal.h"
#include <sg_gui/Buttons.h>
#include <sg_gui/Modifiers.h>

namespace gui {

class PenSignal : public PointerSignal {

public:

	typedef PointerSignal parent_type;

	PenSignal() :
		pressure(0.75),
		modifiers(NoModifier) {}

	PenSignal(
			unsigned long timestamp,
			const util::point<double>& position_,
			double pressure_,
			const Modifiers& modifiers_) :
		PointerSignal(timestamp, position_),
		pressure(pressure_),
		modifiers(modifiers_) {}

	double pressure;

	Modifiers modifiers;
};

class PenMove : public PenSignal {

public:

	typedef PenSignal parent_type;

	PenMove() {}

	PenMove(
			unsigned long timestamp,
			const util::point<double>& position,
			double pressure,
			const Modifiers& modifiers) :
		PenSignal(timestamp, position, pressure, modifiers) {}
};

class PenDown : public PenSignal {

public:

	typedef PenSignal parent_type;

	PenDown() :
		button(buttons::NoButton) {}

	PenDown(
			unsigned long timestamp,
			const buttons::Button& button_,
			const util::point<double>& position,
			double pressure,
			const Modifiers& modifiers) :
		PenSignal(timestamp, position, pressure, modifiers),
		button(button_) {}

	buttons::Button button;
};

class PenUp   : public PenSignal {

public:

	typedef PenSignal parent_type;

	PenUp() {}

	PenUp(
			unsigned long timestamp,
			const buttons::Button& button_,
			const util::point<double>& position,
			double pressure,
			const Modifiers& modifiers) :
		PenSignal(timestamp, position, pressure, modifiers),
		button(button_) {}

	buttons::Button button;
};

class PenIn : public PenSignal {

public:

	typedef PenSignal parent_type;

	PenIn() {}

	PenIn(unsigned long timestamp) :
		PenSignal(
				timestamp,
				util::point<double>(0, 0),
				0,
				gui::NoModifier) {}
};

class PenOut : public PenSignal {

public:

	typedef PenSignal parent_type;

	PenOut() {}

	PenOut(unsigned long timestamp) :
		PenSignal(
				timestamp,
				util::point<double>(0, 0),
				0,
				gui::NoModifier) {}
};

class PenAway : public PenSignal {

public:

	typedef PenSignal parent_type;

	PenAway() {}

	PenAway(unsigned long timestamp) :
		PenSignal(
				timestamp,
				util::point<double>(0, 0),
				0,
				gui::NoModifier) {}
};


} // namespace gui

#endif // GUI_PEN_SIGNALS_H__

