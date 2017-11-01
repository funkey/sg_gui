#include "OpenGl.h"
#include "Colors.h"
#include "MeshView.h"
#include "MarchingCubes.h"
#include <util/ProgramOptions.h>
#include <util/Logger.h>
#include <util/geometry.hpp>
#include <fstream>

logger::LogChannel meshviewlog("meshviewlog", "[MeshView] ");

util::ProgramOption optionCubeSize(
		util::_long_name        = "cubeSize",
		util::_description_text = "The size of a cube for the marching cubes visualization.",
		util::_default_value    = 10);

util::ProgramOption optionMaxNumThreads(
		util::_long_name        = "maxNumThreads",
		util::_description_text = "The maximal number of threads to use for mesh extraction.",
		util::_default_value    = 10);

namespace sg_gui {

MeshView::MeshView(std::shared_ptr<ExplicitVolume<uint64_t>> labels) :
	_labels(labels),
	_meshes(std::make_shared<Meshes>()),
	_minCubeSize(optionCubeSize),
	_alpha(1.0),
	_haveAlphaPlane(false),
	_numThreads(0),
	_maxNumThreads(optionMaxNumThreads) {}
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
MeshView::onSignal(ShowSegment& signal) {

	unsigned int label = signal.getId();

	LOG_USER(meshviewlog) << "showing label " << label << std::endl;

	{
		LockGuard guard(*_meshes);

		if (_meshCache.count(label)) {

			_meshes->add(label, _meshCache[label]);

			updateRecording();
			send<ContentChanged>();

			return;
		}
	}

	typedef ExplicitVolumeLabelAdaptor<ExplicitVolume<uint64_t>> Adaptor;

	for (float downsample : {1}) {
	//for (float downsample : {32, 16, 8, 4, 2, 1}) {

		auto extractMesh =
				std::packaged_task<std::shared_ptr<sg_gui::Mesh>()>(
						[this, label, downsample]() {

							Adaptor adaptor(*this->_labels, label);
							float cubeSize = this->_minCubeSize*downsample;

							sg_gui::MarchingCubes<Adaptor> marchingCubes;
							std::shared_ptr<sg_gui::Mesh> mesh = marchingCubes.generateSurface(
									adaptor,
									sg_gui::MarchingCubes<Adaptor>::AcceptAbove(0),
									cubeSize,
									cubeSize,
									cubeSize);

							this->notifyMeshExtracted(mesh, label);

							return mesh;
						}
				);

		if (downsample == 1)
			_highresMeshFutures.push_back(extractMesh.get_future());

		// don't overdo it...
		while (_numThreads > _maxNumThreads)
			usleep(1000);

		std::thread(std::move(extractMesh)).detach();

		_numThreads++;
	}
}

void
MeshView::onSignal(HideSegment& signal) {

	LockGuard guard(*_meshes);

	_meshes->remove(signal.getId());

	updateRecording();
	send<ContentChanged>();
}

void
MeshView::notifyMeshExtracted(std::shared_ptr<sg_gui::Mesh> mesh, uint64_t label) {

	LockGuard guard(*_meshes);

	LOG_USER(meshviewlog) << "finished mesh for " << label << std::endl;

	// don't replace existing mesh with lower resolution mesh
	if (_meshes->contains(label))
		if (_meshes->get(label)->getNumVertices() > mesh->getNumVertices())
			return;

	_meshes->add(label, mesh);
	_meshCache[label] = mesh;

	updateRecording();
	send<ContentChanged>();

	LOG_USER(meshviewlog) << "added mesh " << label << std::endl;

	_numThreads--;
}

void
MeshView::onSignal(KeyDown& signal) {

	if (signal.key == sg_gui::keys::F8) {

		LOG_USER(meshviewlog) << "exporting currently visible meshes" << std::endl;

		exportMeshes();
	}
}

void
MeshView::exportMeshes() {

	std::vector<uint64_t> currentMeshIds;
	std::vector<std::future<std::shared_ptr<sg_gui::Mesh>>> pendingFutures;

	{
		LockGuard guard(*_meshes);

		currentMeshIds = _meshes->getMeshIds();

		// get all currently pending high-res mesh futures
		std::swap(pendingFutures, _highresMeshFutures);
	}

	LOG_USER(meshviewlog) << "waiting for all high-res meshes to complete..." << std::endl;

	// make sure all pending high-res meshes are done
	for (auto& future : pendingFutures)
		future.get();

	LOG_USER(meshviewlog) << "all high-res meshes completed" << std::endl;

	LockGuard guard(*_meshes);

	for (uint64_t id : currentMeshIds) {

		LOG_USER(meshviewlog) << "exporting mesh " << id << std::endl;

		std::stringstream filename;
		filename << "mesh_" << id << ".raw";

		std::ofstream file(filename.str().c_str());
		std::shared_ptr<sg_gui::Mesh> mesh = _meshes->get(id);

		for (int i = 0; i < mesh->getNumTriangles(); i++) {

			const sg_gui::Triangle& triangle = mesh->getTriangle(i);

			for (const sg_gui::Point3d& v : {
					mesh->getVertex(triangle.v0),
					mesh->getVertex(triangle.v1),
					mesh->getVertex(triangle.v2) }) {

				file << v.x() << " " << v.y() << " " << v.z() << " ";
			}
			file << std::endl;
		}
	}
}

void
MeshView::updateRecording() {

	return;

	if (!_meshes)
		return;

	LockGuard meshGuard(*_meshes);
	OpenGl::Guard guard;

	startRecording();

	glPushMatrix();
	glTranslatef(_offset.x(), _offset.y(), _offset.z());

	foreach (uint64_t id, _meshes->getMeshIds()) {

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
