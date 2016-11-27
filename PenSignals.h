#ifndef SG_GUI_PEN_SIGNALS_H__
#define SG_GUI_PEN_SIGNALS_H__

#include "PointerSignals.h"
#include <sg_gui/Buttons.h>
#include <sg_gui/Modifiers.h>

namespace sg_gui {

class PenMove : public PointerMove {

public:

	typedef PointerMove parent_type;

	PenMove(
			unsigned long timestamp,
			const util::point<float,2>& position,
			float pressure_,
			const Modifiers& modifiers) :
		PointerMove(timestamp, position, modifiers),
		pressure(pressure_) {}

	float pressure;
};

class PenDown : public PointerDown {

public:

	typedef PointerDown parent_type;

	PenDown(
			unsigned long timestamp,
			const buttons::Button& button,
			const util::point<float,2>& position,
			float pressure_,
			const Modifiers& modifiers) :
		PointerDown(timestamp, position, modifiers, button),
		pressure(pressure_) {}

	float pressure;
};

class PenUp : public PointerUp {

public:

	typedef PointerUp parent_type;

	PenUp(
			unsigned long timestamp,
			const buttons::Button& button,
			const util::point<float,2>& position,
			float pressure_,
			const Modifiers& modifiers) :
		PointerUp(timestamp, position, modifiers, button),
		pressure(pressure_) {}

	float pressure;
};

class PenIn : public PointerSignal {

public:

	typedef PointerSignal parent_type;

	PenIn(unsigned long timestamp) :
		PointerSignal(
				timestamp,
				util::point<float,2>(0, 0),
				sg_gui::NoModifier) {}
};

class PenOut : public PointerSignal {

public:

	typedef PointerSignal parent_type;

	PenOut(unsigned long timestamp) :
		PointerSignal(
				timestamp,
				util::point<float,2>(0, 0),
				sg_gui::NoModifier) {}
};

class PenAway : public PointerSignal {

public:

	typedef PointerSignal parent_type;

	PenAway(unsigned long timestamp) :
		PointerSignal(
				timestamp,
				util::point<float,2>(0, 0),
				sg_gui::NoModifier) {}
};


} // namespace sg_gui

#endif // SG_GUI_PEN_SIGNALS_H__

