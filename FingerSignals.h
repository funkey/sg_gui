#ifndef SG_GUI_FINGER_SIGNALS_H__
#define SG_GUI_FINGER_SIGNALS_H__

#include "PointerSignals.h"
#include "Buttons.h"
#include "Modifiers.h"

namespace sg_gui {

class FingerMove : public PointerMove {

public:

	typedef PointerMove parent_type;

	FingerMove(
			unsigned long timestamp,
			const util::point<float,2>& position,
			int id_,
			const Modifiers& modifiers) :
		PointerMove(timestamp, position, modifiers),
		id(id_) {}

	int id;
};

class FingerDown : public PointerDown {

public:

	typedef PointerDown parent_type;

	FingerDown(
			unsigned long timestamp,
			const util::point<float,2>& position,
			int id_,
			const Modifiers& modifiers) :
		PointerDown(timestamp, position, modifiers, buttons::Left),
		id(id_) {}

	int id;
};

class FingerUp : public PointerUp {

public:

	typedef PointerUp parent_type;

	FingerUp(
			unsigned long timestamp,
			const util::point<float,2>& position,
			int id_,
			const Modifiers& modifiers) :
		PointerUp(timestamp, position, modifiers, buttons::Left),
		id(id_) {}

	int id;
};

} // namespace sg_gui

#endif // SG_GUI_FINGER_SIGNALS_H__


