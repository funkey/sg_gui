#include <cmath>
#include <util/Logger.h>
#include <util/geometry.hpp>
#include "OpenGl.h"
#include "RotateView.h"

namespace sg_gui {

static logger::LogChannel rotateviewlog("rotateviewlog", "[RotateView] ");

RotateView::RotateView() :
	_x(0.0),
	_y(1.0),
	_z(0.0),
	_w(0.0),
	_prevX(0.0),
	_prevY(1.0),
	_prevZ(0.0),
	_prevW(0.0),
	_buttonDown(0, 0),
	_dragging(false),
	_highlight(false) {}

bool
RotateView::filterDown(DrawBase& /*signal*/) {

	updateContentSize();

	util::point<float,3> min    = _contentSize.min();
	util::point<float,3> max    = _contentSize.max();
	util::point<float,3> center = _contentSize.center();

	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();

	// put a light
	float lightX = -100;
	float lightY = -100;
	float lightZ = -100;
	GLfloat lightpos[] = {lightX, lightY, lightZ, 0.0};
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightpos);
	GLfloat ambient[] = {0.5, 0.5, 0.5, 1.0};
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	GLfloat specular[] = {0.3, 0.3, 0.3, 1.0};
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	GLfloat diffuse[] = {0.5, 0.5, 0.5, 1.0};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);

	// perform the rotation
	glTranslated(center.x(),  center.y(), center.z());
	glRotated(_w/M_PI*180.0, _x, _y, _z);
	glTranslated(-center.x(), -center.y(), -center.z());

	glColor4f((_highlight ? 0.88 : 0.1), 0.2, 0.05, 0.5);

	// draw box around content
	glLineWidth(1.0);
	glEnable(GL_LINE_SMOOTH);

	glBegin(GL_LINES);
	glVertex3f(min.x(), min.y(), min.z());
	glVertex3f(min.x(), min.y(), max.z());

	glVertex3f(min.x(), min.y(), max.z());
	glVertex3f(min.x(), max.y(), max.z());

	glVertex3f(min.x(), max.y(), max.z());
	glVertex3f(min.x(), max.y(), min.z());

	glVertex3f(min.x(), max.y(), min.z());
	glVertex3f(min.x(), min.y(), min.z());

	glVertex3f(max.x(), min.y(), min.z());
	glVertex3f(max.x(), min.y(), max.z());

	glVertex3f(max.x(), min.y(), max.z());
	glVertex3f(max.x(), max.y(), max.z());

	glVertex3f(max.x(), max.y(), max.z());
	glVertex3f(max.x(), max.y(), min.z());

	glVertex3f(max.x(), max.y(), min.z());
	glVertex3f(max.x(), min.y(), min.z());

	glVertex3f(min.x(), min.y(), min.z());
	glVertex3f(max.x(), min.y(), min.z());

	glVertex3f(min.x(), min.y(), max.z());
	glVertex3f(max.x(), min.y(), max.z());

	glVertex3f(min.x(), max.y(), max.z());
	glVertex3f(max.x(), max.y(), max.z());

	glVertex3f(min.x(), max.y(), min.z());
	glVertex3f(max.x(), max.y(), min.z());
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	return true;
}

void
RotateView::unfilterDown(DrawBase& /*signal*/) {

	glDisable(GL_LIGHT1);
	glPopMatrix();
}

bool
RotateView::filterDown(QuerySize& signal) {

	LOG_ALL(rotateviewlog) << "got size query..." << std::endl;

	updateContentSize();

	LOG_ALL(rotateviewlog) << "content size is " << _contentSize << std::endl;

	// bounding box of all possible rotations around center
	float maxDiameter = util::length(_contentSize.max() - _contentSize.min());
	util::point<float,3> center = _contentSize.center();

	util::box<float,3> size(
			center - util::point<float,3>(maxDiameter/2, maxDiameter/2, maxDiameter/2),
			center + util::point<float,3>(maxDiameter/2, maxDiameter/2, maxDiameter/2));

	LOG_ALL(rotateviewlog) << "max rotated content size is " << size << std::endl;

	signal.setSize(size);

	return false;
}

void
RotateView::unfilterDown(QuerySize&) {}

bool
RotateView::filterDown(PointerSignal& signal) {

	signal.ray = unrotate(signal.ray);

	return true;
}

void
RotateView::unfilterDown(PointerSignal& signal) {

	signal.ray = rotate(signal.ray);
}

void
RotateView::onSignal(MouseDown& signal) {

	util::point<float,2> position = signal.ray.position().project<2>();

	LOG_ALL(rotateviewlog) << "mouse button " << signal.button << " down, position is " << position << std::endl;

	if (signal.button == buttons::Left && intersectsContent(unrotate(signal.ray))) {

		LOG_ALL(rotateviewlog) << "it's the left mouse button -- start dragging mode" << std::endl;

		_dragging = true;
		_buttonDown = position;

		_prevX = _x;
		_prevY = _y;
		_prevZ = _z;
		_prevW = _w;
	}
}

void
RotateView::onSignal(MouseMove& signal) {

	LOG_ALL(rotateviewlog) << "the mouse is moved" << std::endl;

	if (!signal.modifiers & buttons::LeftDown) {

		bool wasHighlighted = _highlight;

		_highlight = intersectsContent(unrotate(signal.ray));

		if (wasHighlighted != _highlight)
			send<ContentChanged>();
	}

	if (!_dragging)
		return;

	LOG_ALL(rotateviewlog) << "I am in dragging mode" << std::endl;

	// mouse is dragged
	if (signal.modifiers & buttons::LeftDown) {

		LOG_ALL(rotateviewlog) << "left button is still pressed" << std::endl;

		util::point<float,2> moved = signal.ray.position().project<2>() - _buttonDown;

		// scale movement with inverse size of content
		moved.x() /= _contentSize.width();
		moved.y() /= _contentSize.height();

		// change the current rotation according to mouse movement
		updateRotationFrom2D(moved);

		send<ContentChanged>();

	} else {

		LOG_ALL(rotateviewlog) << "left button released -- stop dragging" << std::endl;

		_dragging = false;
	}
}

void
RotateView::onInnerSignal(ContentChanged&) {

	_contentSize = util::box<float,3>();
	updateContentSize();
}

void
RotateView::updateContentSize() {

	LOG_ALL(rotateviewlog) << "updating content size..." << std::endl;

	if (!_contentSize.isZero())
		return;

	QuerySize query;
	sendInner(query);
	_contentSize = query.getSize();
}

void
RotateView::updateRotationFrom2D(const util::point<float,2>& moved) {

	LOG_ALL(rotateviewlog) << "current rotation: " << _w << ", (" << _x << ", " << _y << ", " << _z << ")" << std::endl;

	LOG_ALL(rotateviewlog) << "moved by: " << moved << std::endl;

	// previous rotation to quaternion
	float qx, qy, qz, qw;

	qx = _prevX*sin(_prevW/2.0);
	qy = _prevY*sin(_prevW/2.0);
	qz = _prevZ*sin(_prevW/2.0);
	qw =        cos(_prevW/2.0);

	// difference rotation as angle-axis
	float dx, dy, dz, dw;

	// ensure numerical stability
	float dnorm = sqrt(moved.y()*moved.y() + moved.x()*moved.x());
	if (dnorm <= 0.0001)
		return;

	dx =  moved.y()/dnorm;
	dy = -moved.x()/dnorm;
	dz = 0.0;
	dw = dnorm*M_PI;

	LOG_ALL(rotateviewlog) << "add rotation: " << dw << ", (" << dx << ", " << dy << ", " << dz << ")" << std::endl;

	// difference rotation to quaternion
	float qdx, qdy, qdz, qdw;

	qdx = dx*sin(dw/2.0);
	qdy = dy*sin(dw/2.0);
	qdz = dz*sin(dw/2.0);
	qdw =    cos(dw/2.0);

	// composition of both quaternions
	float cx, cy, cz, cw;

	cw = (qdw*qw - qdx*qx - qdy*qy - qdz*qz);
	cx = (qdw*qx + qdx*qw + qdy*qz - qdz*qy);
	cy = (qdw*qy - qdx*qz + qdy*qw + qdz*qx);
	cz = (qdw*qz + qdx*qy - qdy*qx + qdz*qw);

	LOG_ALL(rotateviewlog) << "result quaternion: " << cw << ", (" << cx << ", " << cy << ", " << cz << ")" << std::endl;

	// back to axis-angle rotation
	_w = 2*acos(cw);
	_x = cx/sin(_w/2.0);
	_y = cy/sin(_w/2.0);
	_z = cz/sin(_w/2.0);

	// normalize rotation vector
	float norm = sqrt(_x*_x + _y*_y + _z*_z);
	_x = _x/norm;
	_y = _y/norm;
	_z = _z/norm;

	LOG_ALL(rotateviewlog) << "new rotation: " << _w << ", (" << _x << ", " << _y << ", " << _z << ")" << std::endl;
}

util::ray<float,3>
RotateView::rotate(const util::ray<float,3>& ray) {

	util::ray<float,3> rotated;

	// inversely rotate ray position around content center
	rotated.position() = util::rotate3d(
			util::ray<float,3>(
					_contentSize.center(),
					util::point<float,3>(_x, _y, _z)),
			_w,
			ray.position());

	// inversely rotate ray direction around (0,0,0)
	rotated.direction() = util::rotate3d(
			util::ray<float,3>(
					util::point<float,3>( 0,  0,  0),
					util::point<float,3>(_x, _y, _z)),
			_w,
			ray.direction());

	return rotated;
}

util::ray<float,3>
RotateView::unrotate(const util::ray<float,3>& ray) {

	util::ray<float,3> unrotated;

	// rotate ray position around content center
	unrotated.position() = util::rotate3d(
			util::ray<float,3>(
					_contentSize.center(),
					util::point<float,3>(_x, _y, _z)),
			-_w,
			ray.position());

	// rotate ray direction around (0,0,0)
	unrotated.direction() = util::rotate3d(
			util::ray<float,3>(
					util::point<float,3>( 0,  0,  0),
					util::point<float,3>(_x, _y, _z)),
			-_w,
			ray.direction());

	return unrotated;
}

bool
RotateView::intersectsContent(const util::ray<float,3>& ray) {

	float _;
	return intersect(_contentSize, ray, _);
}

} // namespace sg_gui
