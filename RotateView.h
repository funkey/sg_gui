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
		sg::AcceptsInner<
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
				QuerySize,
				PointerSignal
		>> {

public:

	RotateView();

	bool filterDown(DrawBase& draw);
	void unfilterDown(DrawBase& draw);
	bool filterDown(QuerySize& signal);
	void unfilterDown(QuerySize& signal);
	bool filterDown(PointerSignal& signal);
	void unfilterDown(PointerSignal& signal);
	void onSignal(MouseDown& signal);
	void onSignal(MouseMove& signal);
	void onInnerSignal(ContentChanged& signal);

private:

	void updateContentSize();

	void updateRotationFrom2D(const util::point<float,2>& moved);

	util::ray<float,3> rotate(const util::ray<float,3>& ray);
	util::ray<float,3> unrotate(const util::ray<float,3>& ray);

	bool intersectsContent(const util::ray<float,3>& ray);

	// the current rotation parameters
	float _x, _y, _z, _w;

	// the rotation parameters when the mouse was pressed
	float _prevX, _prevY, _prevZ, _prevW;

	// remember the last mouse position
	util::point<float,2> _buttonDown;

	// indicate that we are in dragging mode
	bool _dragging;

	// the bounding box of all views below this scope
	util::box<float,3> _contentSize;

	// hightlight on mouse over
	bool _highlight;
};

} // namespace sg_gui

#endif // SG_GUI_ROTATE_VIEW_H__

