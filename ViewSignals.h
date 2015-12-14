#ifndef SG_GUI_VIEW_SIGNALS_H__
#define SG_GUI_VIEW_SIGNALS_H__

#include "GuiSignals.h"
#include <util/plane.hpp>

namespace sg_gui {

class ChangeAlpha : public GuiSignal {

public:

	typedef GuiSignal parent_type;

	ChangeAlpha(double alpha_) :
		alpha(alpha_) {}

	double alpha;
};

class SetAlphaPlane : public ChangeAlpha {

public:

	typedef ChangeAlpha parent_type;

	SetAlphaPlane(
			double alpha_,
			const util::plane<double,3>& plane_,
			double falloff_) :
		ChangeAlpha(alpha_),
		plane(plane_),
		falloff(falloff_) {}

	util::plane<double,3> plane;
	double falloff;
};

} // namespace sg_gui

#endif // SG_GUI_VIEW_SIGNALS_H__

