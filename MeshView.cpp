#include "OpenGl.h"
#include "Colors.h"
#include "MeshView.h"
#include <util/geometry.hpp>

namespace sg_gui {

void
MeshView::setMeshes(std::shared_ptr<Meshes> meshes) {

	_meshes = meshes;
	updateRecording();

	send<ContentChanged>();
}

void
MeshView::setOffset(util::point<float, 3> offset) {

	_offset = offset;
	send<ContentChanged>();
}

void
MeshView::onSignal(DrawOpaque& /*draw*/) {

	if (_alpha < 1.0)
		return;

	draw();
}

void
MeshView::onSignal(DrawTranslucent& /*draw*/) {

	if (_alpha == 1.0 || _alpha == 0.0)
		return;

	draw();
}

void
MeshView::onSignal(QuerySize& signal) {

	if (!_meshes)
		return;

	LockGuard guard(*_meshes);

	signal.setSize(_meshes->getBoundingBox() + _offset);
}

void
MeshView::onSignal(ChangeAlpha& signal) {

	_alpha = signal.alpha;
	_haveAlphaPlane = false;

	updateRecording();

	send<ContentChanged>();
}

void
MeshView::onSignal(SetAlphaPlane& signal) {

	_alpha        = signal.alpha;
	_alphaPlane   = signal.plane;
	_alphaFalloff = signal.falloff;
	_haveAlphaPlane = true;

	updateRecording();

	send<ContentChanged>();
}

void
MeshView::updateRecording() {

	if (!_meshes)
		return;

	LockGuard guard(*_meshes);

	OpenGl::Guard glguard;

	startRecording();

	glPushMatrix();
	glTranslatef(_offset.x(), _offset.y(), _offset.z());

	foreach (unsigned int id, _meshes->getMeshIds()) {

		// colorize the mesh according to its id
		unsigned char cr, cg, cb;
		idToRgb(_meshes->getColor(id), cr, cg, cb);
		float r = static_cast<float>(cr)/255.0;
		float g = static_cast<float>(cg)/255.0;
		float b = static_cast<float>(cb)/255.0;

		const std::vector<Triangle>& triangles = _meshes->get(id)->getTriangles();

		glBegin(GL_TRIANGLES);
		foreach (const Triangle& triangle, triangles) {

			const Point3d&  v0 = _meshes->get(id)->getVertex(triangle.v0);
			const Point3d&  v1 = _meshes->get(id)->getVertex(triangle.v1);
			const Point3d&  v2 = _meshes->get(id)->getVertex(triangle.v2);
			const Vector3d& n0 = _meshes->get(id)->getNormal(triangle.v0);
			const Vector3d& n1 = _meshes->get(id)->getNormal(triangle.v1);
			const Vector3d& n2 = _meshes->get(id)->getNormal(triangle.v2);

			setVertexAlpha(v0, r, g, b);
			glNormal3f(n0.x(), n0.y(), n0.z()); glVertex3f(v0.x(), v0.y(), v0.z());
			setVertexAlpha(v1, r, g, b);
			glNormal3f(n1.x(), n1.y(), n1.z()); glVertex3f(v1.x(), v1.y(), v1.z());
			setVertexAlpha(v2, r, g, b);
			glNormal3f(n2.x(), n2.y(), n2.z()); glVertex3f(v2.x(), v2.y(), v2.z());
		}
		glEnd();
	}

	glPopMatrix();

	stopRecording();
}

void
MeshView::setVertexAlpha(const Point3d& p, float r, float g, float b) {

	if (_haveAlphaPlane) {

		double alpha = 1.0 - std::abs(distance(_alphaPlane, util::point<float,3>(p.x(), p.y(), p.z()))*_alphaFalloff);
		glColor4f(r, g, b, _alpha*alpha);

	} else {

		glColor4f(r, g, b, _alpha);
	}
}

} // namespace sg_gui
