#ifndef SG_GUI_VIEW_SIGNALS_H__
#define SG_GUI_VIEW_SIGNALS_H__

#include "GuiSignals.h"

namespace sg_gui {

class ChangeAlpha : public GuiSignal {

public:

	ChangeAlpha(double alpha_) :
		alpha(alpha_) {}

	double alpha;
};

} // namespace sg_gui

#endif // SG_GUI_VIEW_SIGNALS_H__

