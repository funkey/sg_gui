#include <cmath>
#include <util/Logger.h>
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
RotateView::filterDown(Draw& /*signal*/) {

	if (_contentSize.isZero()) {

		QuerySize query;
		sendInner(query);
		_contentSize = query.getSize();
	}

	util::point<double> ul(_contentSize.minX, _contentSize.minY);
	util::point<double> lr(_contentSize.maxX, _contentSize.maxY);

	util::point<double> center = (ul + lr)/2.0;

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
	glTranslated(center.x,  center.y, 0);
	glRotated(_w/M_PI*180.0, _x, _y, _z);
	glTranslated(-center.x, -center.y, 0);

	glColor4f((_highlight ? 0.88 : 0.1), 0.2, 0.05, 0.5);

	// draw 2d frame around content
	glCheck(glEnable(GL_BLEND));
	glCheck(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	glLineWidth(2.0);
	glEnable(GL_LINE_SMOOTH);

	glBegin(GL_LINES);
	glVertex2f(ul.x, ul.y);
	glVertex2f(lr.x, ul.y);

	glVertex2f(lr.x, ul.y);
	glVertex2f(lr.x, lr.y);

	glVertex2f(lr.x, lr.y);
	glVertex2f(ul.x, lr.y);

	glVertex2f(ul.x, lr.y);
	glVertex2f(ul.x, ul.y);
	glEnd();

	// draw solid backside
	glCheck(glEnable(GL_CULL_FACE));
	glCheck(glEnable(GL_LIGHTING));
	glCheck(glEnable(GL_COLOR_MATERIAL));

	glBegin(GL_QUADS);
	glVertex2f(ul.x, ul.y);
	glVertex2f(lr.x, ul.y);
	glVertex2f(lr.x, lr.y);
	glVertex2f(ul.x, lr.y);
	glEnd();

	return true;
}

void
RotateView::unfilterDown(Draw& /*signal*/) {

	glDisable(GL_LIGHT1);
	glPopMatrix();
}

void
RotateView::onSignal(MouseDown& signal) {

	util::point<double> position = signal.position;

	LOG_ALL(rotateviewlog) << "mouse button " << signal.button << " down, position is " << position << std::endl;

	if (signal.button == buttons::Left && _contentSize.contains(position)) {

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

		_highlight = _contentSize.contains(signal.position);

		if (wasHighlighted != _highlight)
			send<ContentChanged>();
	}

	if (!_dragging)
		return;

	LOG_ALL(rotateviewlog) << "I am in dragging mode" << std::endl;

	// mouse is dragged
	if (signal.modifiers & buttons::LeftDown) {

		LOG_ALL(rotateviewlog) << "left button is still pressed" << std::endl;

		util::point<double> moved = signal.position - _buttonDown;

		// scale movement with inverse size of content
		moved.x /= _contentSize.width();
		moved.y /= _contentSize.height();

		// change the current rotation according to mouse movement
		rotate(moved);

		send<ContentChanged>();

	} else {

		LOG_ALL(rotateviewlog) << "left button released -- stop dragging" << std::endl;

		_dragging = false;
	}
}

void
RotateView::rotate(const util::point<double>& moved) {

	LOG_ALL(rotateviewlog) << "current rotation: " << _w << ", (" << _x << ", " << _y << ", " << _z << ")" << std::endl;

	LOG_ALL(rotateviewlog) << "moved by: (" << moved.x << ", " << moved.y << ")" << std::endl;

	// previous rotation to quaternion
	double qx, qy, qz, qw;

	qx = _prevX*sin(_prevW/2.0);
	qy = _prevY*sin(_prevW/2.0);
	qz = _prevZ*sin(_prevW/2.0);
	qw =        cos(_prevW/2.0);

	// difference rotation as angle-axis
	double dx, dy, dz, dw;

	// ensure numerical stability
	double dnorm = sqrt(moved.y*moved.y + moved.x*moved.x);
	if (dnorm <= 0.0001)
		return;

	dx =  moved.y/dnorm;
	dy = -moved.x/dnorm;
	dz = 0.0;
	dw = dnorm*M_PI;

	LOG_ALL(rotateviewlog) << "add rotation: " << dw << ", (" << dx << ", " << dy << ", " << dz << ")" << std::endl;

	// difference rotation to quaternion
	double qdx, qdy, qdz, qdw;

	qdx = dx*sin(dw/2.0);
	qdy = dy*sin(dw/2.0);
	qdz = dz*sin(dw/2.0);
	qdw =    cos(dw/2.0);

	// composition of both quaternions
	double cx, cy, cz, cw;

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
	double norm = sqrt(_x*_x + _y*_y + _z*_z);
	_x = _x/norm;
	_y = _y/norm;
	_z = _z/norm;

	LOG_ALL(rotateviewlog) << "new rotation: " << _w << ", (" << _x << ", " << _y << ", " << _z << ")" << std::endl;
}

} // namespace sg_gui
