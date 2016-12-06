#include <sstream>
#include <util/Logger.h>
#include <util/ProgramOptions.h>
#include "OpenGl.h"
#include "ZoomView.h"

util::ProgramOption optionZoomInitialScale(
		util::_module           = "gui",
		util::_long_name        = "zoomInitialScale",
		util::_description_text = "The initial scale.",
		util::_default_value    = 1.0);

util::ProgramOption optionZoomInitialShiftX(
		util::_module           = "gui",
		util::_long_name        = "zoomInitialShiftX",
		util::_description_text = "The initial shift as (x,y).",
		util::_default_value    = 0.0);

util::ProgramOption optionZoomInitialShiftY(
		util::_module           = "gui",
		util::_long_name        = "zoomInitialShiftY",
		util::_description_text = "The initial shift as (x,y).",
		util::_default_value    = 0.0);

namespace sg_gui {

static logger::LogChannel zoomviewlog("zoomviewlog", "[ZoomView] ");

ZoomView::ZoomView(bool autoscale) :
	_zoomStep(1.1),
	_dragging(false),
	_autoscale(autoscale),
	_userScale(optionZoomInitialScale),
	_userShift(optionZoomInitialShiftX, optionZoomInitialShiftY),
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

	_zoomedRoi = (signal.roi() - _shift)/_scale;

	/* To obtain proper perspective deformations, we set the frustum such that 
	 * the vanishing point is in the middle of the zoomed roi. During drawing, 
	 * we change the modelview matrix to compensate for that, such that zoomed 
	 * roi upper left is in the frustum upper left.
	 */

	/* Next, we simulate the zoom by modifying the frustum: Instead of scaling 
	 * the world coordinates, we inversely scale the frustum by using the width 
	 * and height of the zoomed roi.
	 */
	float l2d = - _zoomedRoi.width()/2;
	float r2d = l2d + _zoomedRoi.width();
	float t2d = - _zoomedRoi.height()/2;
	float b2d = t2d + _zoomedRoi.height();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glFrustum(
			l2d*_zClipNear/_z2d, // left
			r2d*_zClipNear/_z2d, // right
			b2d*_zClipNear/_z2d, // bottom
			t2d*_zClipNear/_z2d, // top
			_zClipNear,          // near
			_zClipFar            // far
	);

	/* It remains to invert the z-axis to obtain the coordinate system as 
	 * described above. This is done here, together with translating the world 
	 * coordinates z=0 to z=_z2d and compensating for the upper left of the 
	 * zoomed roi.
	 */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-_zoomedRoi.min().x() - _zoomedRoi.width()/2, -_zoomedRoi.min().y() - _zoomedRoi.height()/2, -_z2d);
	glScalef(1, 1, -1);

	signal.roi() = _zoomedRoi;
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

	signal.ray.position() -= _shift.project<3>();
	signal.ray.position() /= _scale;

	// in 3D on 2d plane
	util::point<float,3> p3d = signal.ray.position();

	// relative to roi center
	p3d -= _zoomedRoi.center().project<3>();
	p3d.z() = _z2d;

	util::point<float,3> direction = p3d;
	direction /= sqrt(
			direction.x()*direction.x() +
			direction.y()*direction.y() +
			direction.z()*direction.z());

	signal.ray.direction() = direction;

	return true;
}

void
ZoomView::unfilterDown(PointerSignal& signal) {

	signal.ray.position() *= _scale;
	signal.ray.position() += _shift.project<3>();
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

	util::point<float,2> position = signal.ray.position().project<2>();

	LOG_ALL(zoomviewlog) << "mouse button " << signal.button << " down, position is " << position << std::endl;

	if (signal.button == buttons::Left) {

		LOG_ALL(zoomviewlog) << "it's the left mouse button -- start dragging mode" << std::endl;

		_dragging = true;
		_buttonDown = position;

		return;
	}

	// if shift is pressed, increase zoom speed
	float zoomStep = _zoomStep;
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

	float amp = 1.0;
	if (signal.modifiers & keys::ShiftDown)
		amp = 10.0;

	// mouse is dragged
	if (signal.modifiers & buttons::LeftDown) {

		LOG_ALL(zoomviewlog) << "left button is still pressed" << std::endl;

		util::point<float,2> moved = signal.ray.position().project<2>() - _buttonDown;

		drag(moved*amp);

		_buttonDown = signal.ray.position().project<2>();

		send<ContentChanged>();

	} else {

		LOG_ALL(zoomviewlog) << "left button released -- stop dragging" << std::endl;

		_dragging = false;
	}
}

void
ZoomView::onSignal(KeyDown& signal) {

	if (signal.key == keys::R) {

		_userScale = 1;
		_userShift = util::point<float, 2>(0, 0);

		updateScaleAndShift();

		send<ContentChanged>();
	}

	if (signal.key == keys::T) {

		if (signal.modifiers & keys::ShiftDown) {

			LOG_USER(zoomviewlog) << "enter zoom and shift values as \"<zoom> (<shift_x>, <shift_y>)\": " << std::flush;

			char input[256];
			std::cin.getline(input, 256);
			std::stringstream ss(input);

			float z, sx, sy;
			char c;
			ss >> z;
			ss >> c; if (c != '(') { LOG_USER(zoomviewlog) << "expected '('" << std::endl; return; }
			ss >> sx;
			ss >> c; if (c != ',') { LOG_USER(zoomviewlog) << "expected ','" << std::endl; return; }
			ss >> sy;
			ss >> c; if (c != ')') { LOG_USER(zoomviewlog) << "expected ')'" << std::endl; return; }

			_userScale = z;
			_userShift.x() = sx;
			_userShift.y() = sy;

			updateScaleAndShift();

			send<ContentChanged>();

		} else
			LOG_USER(zoomviewlog) << "zoom transformation: " << _userScale << " " << _userShift << std::endl;
	}
}

void
ZoomView::onInnerSignal(ContentChanged&) {

	updateContentSize();
	updateScaleAndShift();
	updateClippingPlanes();
}

void
ZoomView::onInnerSignal(sg::AgentAdded&) {

	updateContentSize();
	updateScaleAndShift();
	updateClippingPlanes();
}

void
ZoomView::zoom(float zoomChange, const util::point<float,2>& anchor) {

	LOG_ALL(zoomviewlog) << "changing user zoom by " << zoomChange << " keeping " << anchor << " where it is" << std::endl;

	_userScale *= zoomChange;
	_userShift  = (anchor - zoomChange*(anchor - (_autoScale*_userShift + _autoShift)) - _autoShift)/_autoScale;

	LOG_ALL(zoomviewlog) << "user zoom is now " << _userScale << ", user shift is " << _userShift << std::endl;

	updateScaleAndShift();

	LOG_ALL(zoomviewlog) << "final zoom is now " << _scale << ", final shift is " << _shift << std::endl;
}

void
ZoomView::drag(const util::point<float,2>& direction) {

	_userShift += direction/_autoScale;

	updateScaleAndShift();
}

void
ZoomView::updateContentSize() {

	QuerySize signal;
	sendInner(signal);
	_contentSize = signal.getSize();
}

void
ZoomView::updateScaleAndShift() {

	_autoScale = 1.0;
	_autoShift = util::point<float,2>(0, 0);

	// first, apply autoscale transformation (if wanted)
	if (_autoscale) {

		// do we have to fit the width or height of the content?
		bool fitHeight = (_contentSize.width()/_contentSize.height() < _desiredSize.width()/_desiredSize.height());

		// get the scale to fit the width or height to the desired size
		_autoScale = (fitHeight ? _desiredSize.height()/_contentSize.height() : _desiredSize.width()/_contentSize.width());

		// get the shift to center the content in the desired area relative to 
		// desired upper left
		util::point<float,2> centerShift =
				(fitHeight ?
				 util::point<float,2>(1, 0)*0.5*(_desiredSize.width()  - _contentSize.width() *_autoScale) :
				 util::point<float,2>(0, 1)*0.5*(_desiredSize.height() - _contentSize.height()*_autoScale));

		// get the final shift relative to content upper left
		util::point<float,2> ul(_contentSize.min().x(), _contentSize.min().y());
		_autoShift = (_desiredSize.min() - ul)*_autoScale + centerShift;
	}

	// append user scale and shift transformation
	_shift = _autoScale*_userShift + _autoShift;
	_scale = _autoScale*_userScale;
}

void
ZoomView::updateClippingPlanes() {

	/**
	 * Setting the clipping planes determines our field-of-view. Here, we set 
	 * them such that the non-user-scaled perspective has a field-of-view of 
	 * 90°. For that, we require maxExtent = max(top, left) = max(bottom, right) 
	 * = 0.5*_zClipNear.
	 */

	float maxExtent = std::max(_contentSize.width(), _contentSize.height())/2.0;

	_zClipNear = 2*maxExtent;
	_zClipFar  = _zClipNear + std::max(10.0, 2*_contentSize.max().z() + 1.0);
	_z2d       = (_zClipFar + _zClipNear)/2.0;
}

} // namespace sg_gui
