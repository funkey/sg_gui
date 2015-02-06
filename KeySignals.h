#ifndef GUI_KEY_SIGNALS_H__
#define GUI_KEY_SIGNALS_H__

#include "GuiSignals.h"
#include <sg_gui/Keys.h>
#include <sg_gui/Modifiers.h>

namespace sg_gui {

class KeySignal : public InputSignal {

public:

	typedef InputSignal parent_type;

	KeySignal() :
		key(keys::NoKey) {};

	KeySignal(const keys::Key& key_, const Modifiers& modifiers_) :
		key(key_),
		modifiers(modifiers_) {}

	keys::Key key;

	Modifiers modifiers;
};

class KeyDown : public KeySignal {

public:

	typedef KeySignal parent_type;

	KeyDown() {}

	KeyDown(const keys::Key& key, const Modifiers& modifiers) :
		KeySignal(key, modifiers) {}
};

class KeyUp : public KeySignal {

public:

	typedef KeySignal parent_type;

	KeyUp() {}

	KeyUp(const keys::Key& key, const Modifiers& modifiers) :
		KeySignal(key, modifiers) {}
};

} // namespace sg_gui

#endif // SG_GUI_KEY_SIGNALS_H__

