#ifndef SG_GUI_ROTATE_VIEW_H__
#define SG_GUI_ROTATE_VIEW_H__

#include <scopegraph/Scope.h>
#include "GuiSignals.h"
#include "MouseSignals.h"
#include "KeySignals.h"

namespace sg_gui {

class RotateView : public sg::Scope<
		RotateView,
		sg::Accepts<
				MouseDown,
				MouseMove
		>,
		sg::Provides<
				ContentChanged
		>,
		sg::ProvidesInner<
				QuerySize
		>,
		sg::PassesUp<
				ContentChanged
		>,
		sg::FiltersDown<
				DrawBase,
				QuerySize
		>> {

public:

	RotateView();

	bool filterDown(DrawBase& draw);
	void unfilterDown(DrawBase& draw);
	bool filterDown(QuerySize& signal);
	void unfilterDown(QuerySize& signal);
	void onSignal(MouseDown& signal);
	void onSignal(MouseMove& signal);

private:

	void rotate(const util::point<double>& moved);

	// the current rotation parameters
	double _x, _y, _z, _w;

	// the rotation parameters when the mouse was pressed
	double _prevX, _prevY, _prevZ, _prevW;

	// remember the last mouse position
	util::point<double> _buttonDown;

	// indicate that we are in dragging mode
	bool _dragging;

	// the bounding box of all views below this scope
	util::box<double> _contentSize;

	// hightlight on mouse over
	bool _highlight;
};

} // namespace sg_gui

#endif // SG_GUI_ROTATE_VIEW_H__

