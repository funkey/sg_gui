#ifndef SG_GUI_MODIFIERS_H__
#define SG_GUI_MODIFIERS_H__

namespace sg_gui {

enum Modifiers {

	NoModifier,

	__ModifierMask = 63
};

namespace buttons {

static const Modifiers LeftDown   = static_cast<Modifiers>(1);
static const Modifiers MiddleDown = static_cast<Modifiers>(2);
static const Modifiers RightDown  = static_cast<Modifiers>(4);

} // namespace buttons

namespace keys {

static const Modifiers ControlDown = static_cast<Modifiers>(8);
static const Modifiers ShiftDown   = static_cast<Modifiers>(16);
static const Modifiers AltDown     = static_cast<Modifiers>(32);

} // namespace keys

} // namespace sg_gui

#endif // SG_GUI_MODIFIERS_H__

