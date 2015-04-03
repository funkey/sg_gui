#ifndef SG_GUI_ZOOM_VIEW_H__
#define SG_GUI_ZOOM_VIEW_H__

#include <scopegraph/Scope.h>
#include "GuiSignals.h"
#include "PointerSignals.h"

namespace sg_gui {

class ZoomView :
		public sg::Scope<
				ZoomView,
				sg::FiltersDown<RoiSignal, PointerSignal, Resize>,
				sg::PassesUp<ContentChanged>,
				sg::Accepts<PointerDown, PointerMove>,
				sg::Provides<ContentChanged>,
				sg::AcceptsInner<
						ContentChanged,
						sg::AgentAdded
				>,
				sg::ProvidesInner<QuerySize>
		> {

public:

	/**
	 * Default constructor.
	 *
	 * @param autoscale
	 *             If set to true, this view will automatically scale the 
	 *             content to fit the size this view was requested to have. For 
	 *             that, a Resize signal has to be sent to this view.
	 */
	ZoomView(bool autoscale = false);

	bool filterDown(RoiSignal& signal);

	void unfilterDown(RoiSignal& signal);

	bool filterDown(PointerSignal& signal);

	void unfilterDown(PointerSignal& signal);

	bool filterDown(Resize& resize);

	void unfilterDown(Resize&) {}

	void onSignal(PointerDown& signal);

	void onSignal(PointerMove& signal);

	void onInnerSignal(ContentChanged& signal);

	void onInnerSignal(sg::AgentAdded& signal);

private:

	/**
	 * Zoom onto the given point. The current scale will be multiplied by 
	 * 'zoomChange'.
	 */
	void zoom(double zoomChange, const util::point<double,2>& anchor);

	/**
	 * Drag the content in the given direction.
	 */
	void drag(const util::point<double,2>& direction);

	/**
	 * Recalculate scale and shift.
	 */
	void updateScaleAndShift();

	// the speed of zooming
	double _zoomStep;

	// remember the last mouse position
	util::point<double,2> _buttonDown;

	// indicate that we are in dragging mode
	bool _dragging;

	// automatically scale content to fit the desired size (which can be given 
	// on construction or set via a resize signal)
	bool _autoscale;

	// the scale and shift of this view as requested by the user
	double _userScale;
	util::point<double,2> _userShift;

	// the scale and shift of this view to fit the content to the desired 
	// size
	double _autoScale;
	util::point<double,2> _autoShift;

	// the effective scale and shift, taking into account possible autoscaling
	double _scale;
	util::point<double,2> _shift;

	// the desired size of this view
	util::rect<double> _desiredSize;

	// the z-coordinate of the 2D viewing plane, where x and y eye coordinates 
	// map to x and y screen pixel coordinates
	double _z2d;

	// z-coordinates of the near and far clipping plane (should contain _z2d)
	double _zClipNear;
	double _zClipFar;
};

} // namespace sg_gui

#endif // SG_GUI_ZOOM_VIEW_H__

