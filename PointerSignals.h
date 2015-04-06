#ifndef SG_GUI_POINTER_SIGNAL_H__
#define SG_GUI_POINTER_SIGNAL_H__

#include "GuiSignals.h"
#include "Modifiers.h"
#include "Buttons.h"
#include <util/ray.hpp>

namespace sg_gui {

/**
 * Base class for all 2D input signals (mouse, pen, touch, ...). The pointer 
 * position is represented by a ray in 3D. For 2D GUIs, the ray will be located 
 * at (x,y,0) and point in (0,0,1) for pointer signals at coordinates (x,y). 
 * However, perspective views might transform the ray.
 */
class PointerSignal : public InputSignal {

public:

	typedef InputSignal parent_type;

	PointerSignal(
			unsigned long               timestamp,
			const util::point<float,2>& position_,
			const Modifiers&            modifiers_) :
		InputSignal(timestamp),
		ray(
				util::point<float,3>(position_.x(), position_.y(), 0),
				util::point<float,3>(0,             0,             1)),
		modifiers(modifiers_) {}

	util::ray<float,3> ray;

	Modifiers modifiers;
};

class PointerMove : public PointerSignal {

public:

	typedef PointerSignal parent_type;

	PointerMove(
			unsigned long               timestamp,
			const util::point<float,2>& position,
			const Modifiers&            modifiers) :
		PointerSignal(timestamp, position, modifiers) {}
};

class PointerDown : public PointerSignal {

public:

	typedef PointerSignal parent_type;

	PointerDown(
			unsigned long               timestamp,
			const util::point<float,2>& position,
			const Modifiers&            modifiers,
			const buttons::Button&      button_) :
		PointerSignal(timestamp, position, modifiers),
		button(button_) {}

	buttons::Button button;
};

class PointerUp : public PointerSignal {

public:

	typedef PointerSignal parent_type;

	PointerUp(
			unsigned long               timestamp,
			const util::point<float,2>& position,
			const Modifiers&            modifiers,
			const buttons::Button&      button_) :
		PointerSignal(timestamp, position, modifiers),
		button(button_) {}

	buttons::Button button;
};

} // namespace sg_gui

#endif // SG_GUI_POINTER_SIGNAL_H__

