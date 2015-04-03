#include <util/Logger.h>
#include "OpenGl.h"
#include "ZoomView.h"

namespace sg_gui {

static logger::LogChannel zoomviewlog("zoomviewlog", "[ZoomView] ");

ZoomView::ZoomView(bool autoscale) :
	_zoomStep(1.1),
	_dragging(false),
	_autoscale(autoscale),
	_userScale(1.0),
	_userShift(0, 0),
	_autoScale(1.0),
	_autoShift(0, 0),
	_scale(1.0),
	_shift(0, 0),
	_desiredSize(0, 0, 1, 1),
	_z2d(1000),
	_zClipNear(1),
	_zClipFar(2000) {}

bool
ZoomView::filterDown(RoiSignal& signal) {

	/* Here, we configre the gl frustum to match the zoomed roi size. In the 
	 * middle of the frustum, at z = _z2d, we have a plane where the x and y 
	 * coordinates match the pixels of the roi, such that (x, y) = (0, 0) is the 
	 * upper left pixel, if _shift is (0, 0).
	 *
	 * Further, we change the gl coordinate system, such that we obtain a 
	 * right-handed coordinate system where the positive directions of x, y, and 
	 * z point right, down, and back, respectively.
	 */

	LOG_ALL(zoomviewlog) << "transforming with " << _shift << " and " << _scale << std::endl;

	util::rect<double> zoomedRoi = (signal.roi() - _shift)/_scale;

	/* To obtain proper perspective deformations, we set the frustum such that 
	 * the vanishing point is in the middle of the zoomed roi. During drawing, 
	 * we change the modelview matrix to compensate for that, such that zoomed 
	 * roi upper left is in the frustum upper left.
	 */

	/* Next, we simulate the zoom by modifying the frustum: Instead of scaling 
	 * the world coordinates, we inversely scale the frustum by using the width 
	 * and height of the zoomed roi.
	 */
	double l2d = - zoomedRoi.width()/2;
	double r2d = l2d + zoomedRoi.width();
	double t2d = - zoomedRoi.height()/2;
	double b2d = t2d + zoomedRoi.height();

	double z2d   = _z2d/_scale;
	double zNear = _zClipNear/_scale;
	double zFar  = _zClipFar/_scale;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glFrustum(
			l2d*zNear/z2d, // left
			r2d*zNear/z2d, // right
			b2d*zNear/z2d, // bottom
			t2d*zNear/z2d, // top
			zNear,         // near
			zFar           // far
	);

	/* It remains to invert the z-axis to obtain the coordinate system as 
	 * described above. This is done here, together with translating the world 
	 * coordinates z=0 to z=_z2d and compensating for the upper left of the 
	 * zoomed roi.
	 */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-zoomedRoi.minX - zoomedRoi.width()/2, -zoomedRoi.minY - zoomedRoi.height()/2, -z2d);
	glScalef(1, 1, -1);

	signal.roi() = zoomedRoi;
	signal.resolution() = signal.resolution()*_scale;

	return true;
}

void
ZoomView::unfilterDown(RoiSignal& signal) {

	signal.roi() = signal.roi()*_scale + _shift;
	signal.resolution() = signal.resolution()/_scale;

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

bool
ZoomView::filterDown(PointerSignal& signal) {

	signal.position -= _shift;
	signal.position /= _scale;

	return true;
}

void
ZoomView::unfilterDown(PointerSignal& signal) {

	signal.position *= _scale;
	signal.position += _shift;
}

bool
ZoomView::filterDown(Resize& signal) {

	_desiredSize = signal.getSize();

	updateScaleAndShift();
	send<ContentChanged>();

	// don't pass on the signal -- we took care of it
	return false;
}

void
ZoomView::onSignal(PointerDown& signal) {

	LOG_ALL(zoomviewlog) << "a button was pressed" << std::endl;

	if (!(signal.modifiers & keys::ControlDown))
		return;

	// that is for us!
	signal.processed = true;

	util::point<double,2> position = signal.position;

	LOG_ALL(zoomviewlog) << "mouse button " << signal.button << " down, position is " << position << std::endl;

	if (signal.button == buttons::Left) {

		LOG_ALL(zoomviewlog) << "it's the left mouse button -- start dragging mode" << std::endl;

		_dragging = true;
		_buttonDown = position;

		return;
	}

	// if shift is pressed, increase zoom speed
	double zoomStep = _zoomStep;
	if (signal.modifiers & keys::ShiftDown)
		zoomStep *= 2;

	// mouse wheel up
	if (signal.button == buttons::WheelUp) {

		LOG_ALL(zoomviewlog) << "it's the left wheel up" << std::endl;

		zoom(zoomStep, position);
	}

	// mouse wheel down
	if (signal.button == buttons::WheelDown) {

		LOG_ALL(zoomviewlog) << "it's the left wheel down" << std::endl;

		zoom(1.0/zoomStep, position);
	}

	send<ContentChanged>();
}

void
ZoomView::onSignal(PointerMove& signal) {

	if (!(signal.modifiers & keys::ControlDown))
		return;

	if (!_dragging) {

		return;
	}

	// that is for us!
	signal.processed = true;

	LOG_ALL(zoomviewlog) << "I am in dragging mode" << std::endl;

	double amp = 1.0;
	if (signal.modifiers & keys::ShiftDown)
		amp = 10.0;

	// mouse is dragged
	if (signal.modifiers & buttons::LeftDown) {

		LOG_ALL(zoomviewlog) << "left button is still pressed" << std::endl;

		util::point<double,2> moved = signal.position - _buttonDown;

		drag(moved*amp);

		_buttonDown = signal.position;

		send<ContentChanged>();

	} else {

		LOG_ALL(zoomviewlog) << "left button released -- stop dragging" << std::endl;

		_dragging = false;
	}
}

void
ZoomView::onInnerSignal(ContentChanged&) {

	updateScaleAndShift();
}

void
ZoomView::onInnerSignal(sg::AgentAdded&) {

	updateScaleAndShift();
}

void
ZoomView::zoom(double zoomChange, const util::point<double,2>& anchor) {

	LOG_ALL(zoomviewlog) << "changing user zoom by " << zoomChange << " keeping " << anchor << " where it is" << std::endl;

	_userScale *= zoomChange;
	_userShift  = (anchor - zoomChange*(anchor - (_autoScale*_userShift + _autoShift)) - _autoShift)/_autoScale;

	LOG_ALL(zoomviewlog) << "user zoom is now " << _userScale << ", user shift is " << _userShift << std::endl;

	updateScaleAndShift();

	LOG_ALL(zoomviewlog) << "final zoom is now " << _scale << ", final shift is " << _shift << std::endl;
}

void
ZoomView::drag(const util::point<double,2>& direction) {

	_userShift += direction/_autoScale;

	updateScaleAndShift();
}

void
ZoomView::updateScaleAndShift() {

	_autoScale = 1.0;
	_autoShift = util::point<double,2>(0, 0);

	// first, apply autoscale transformation (if wanted)
	if (_autoscale) {

		QuerySize signal;
		sendInner(signal);
		const util::box<double>& contentSize = signal.getSize();

		_zClipNear = std::max(1.0,  _z2d + contentSize.minZ - 1.0);
		_zClipFar  = std::max(10.0, _z2d + contentSize.maxZ + 1.0);
		_z2d       = (_zClipFar + _zClipNear)/2.0;

		// do we have to fit the width or height of the content?
		bool fitHeight = (contentSize.width()/contentSize.height() < _desiredSize.width()/_desiredSize.height());

		// get the scale to fit the width or height to the desired size
		_autoScale = (fitHeight ? _desiredSize.height()/contentSize.height() : _desiredSize.width()/contentSize.width());

		// get the shift to center the content in the desired area relative to 
		// desired upper left
		util::point<double,2> centerShift =
				(fitHeight ?
				 util::point<double,2>(1, 0)*0.5*(_desiredSize.width()  - contentSize.width() *_autoScale) :
				 util::point<double,2>(0, 1)*0.5*(_desiredSize.height() - contentSize.height()*_autoScale));

		// get the final shift relative to content upper left
		util::point<double,2> ul(contentSize.minX, contentSize.minY);
		_autoShift = (_desiredSize.upperLeft() - ul)*_autoScale + centerShift;
	}

	// append user scale and shift transformation
	_shift = _autoScale*_userShift + _autoShift;
	_scale = _autoScale*_userScale;
}

} // namespace sg_gui
