#include "OpenGl.h"
#include "Colors.h"
#include "MeshView.h"

namespace sg_gui {

void
MeshView::setMeshes(std::shared_ptr<Meshes> meshes) {

	_meshes = meshes;
	updateRecording();

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

	std::cout << "[MeshView] content: " << _meshes->getBoundingBox() << std::endl;

	signal.setSize(_meshes->getBoundingBox());
}

void
MeshView::onSignal(ChangeAlpha& signal) {

	_alpha = signal.alpha;
	updateRecording();

	send<ContentChanged>();
}

void
MeshView::updateRecording() {

	if (!_meshes)
		return;

	OpenGl::Guard guard;

	startRecording();

	foreach (unsigned int id, _meshes->getMeshIds()) {

		// colorize the mesh according to its id
		unsigned char r, g, b;
		idToRgb(id, r, g, b);
		glColor4f(
				static_cast<float>(r)/255.0,
				static_cast<float>(g)/255.0,
				static_cast<float>(b)/255.0,
				_alpha);

		const std::vector<Triangle>& triangles = _meshes->get(id)->getTriangles();

		glBegin(GL_TRIANGLES);
		foreach (const Triangle& triangle, triangles) {

			const Point3d&  v0 = _meshes->get(id)->getVertex(triangle.v0);
			const Point3d&  v1 = _meshes->get(id)->getVertex(triangle.v1);
			const Point3d&  v2 = _meshes->get(id)->getVertex(triangle.v2);
			const Vector3d& n0 = _meshes->get(id)->getNormal(triangle.v0);
			const Vector3d& n1 = _meshes->get(id)->getNormal(triangle.v1);
			const Vector3d& n2 = _meshes->get(id)->getNormal(triangle.v2);

			glNormal3f(n0.x(), n0.y(), n0.z()); glVertex3f(v0.x(), v0.y(), v0.z());
			glNormal3f(n1.x(), n1.y(), n1.z()); glVertex3f(v1.x(), v1.y(), v1.z());
			glNormal3f(n2.x(), n2.y(), n2.z()); glVertex3f(v2.x(), v2.y(), v2.z());
		}
		glEnd();
	}

	stopRecording();
}

} // namespace sg_gui
