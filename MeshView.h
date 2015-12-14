#ifndef SG_GUI_MESH_VIEW_H__
#define SG_GUI_MESH_VIEW_H__

#include <scopegraph/Agent.h>
#include "GuiSignals.h"
#include "ViewSignals.h"
#include "RecordableView.h"
#include "Meshes.h"

namespace sg_gui {

class SetMeshes : public SetContent {

public:

	typedef SetContent parent_type;

	SetMeshes(std::shared_ptr<Meshes> meshes) :
			_meshes(meshes) {}

	std::shared_ptr<Meshes> getMeshes() { return _meshes; }

private:

	std::shared_ptr<Meshes> _meshes;
};

class MeshView :
		public sg::Agent<
			MeshView,
			sg::Accepts<
					SetMeshes,
					DrawOpaque,
					DrawTranslucent,
					QuerySize,
					ChangeAlpha,
					SetAlphaPlane
			>,
			sg::Provides<
					ContentChanged
			>
		>,
		public RecordableView {

public:

	MeshView() : _alpha(1.0), _haveAlphaPlane(false) {}

	void setMeshes(std::shared_ptr<Meshes> meshes);

	void setOffset(util::point<float, 3> offset);

	void onSignal(SetMeshes& signal) { setMeshes(signal.getMeshes()); }

	void onSignal(DrawOpaque& signal);

	void onSignal(DrawTranslucent& signal);

	void onSignal(QuerySize& signal);

	void onSignal(ChangeAlpha& signal);

	void onSignal(SetAlphaPlane& signal);

private:

	void updateRecording();

	void setVertexAlpha(const Point3d& p, float r, float g, float b);

	std::shared_ptr<Meshes> _meshes;

	double _alpha;
	util::plane<float, 3> _alphaPlane;
	double _alphaFalloff;
	bool _haveAlphaPlane;

	util::point<float, 3> _offset;
};

} // namespace sg_gui

#endif // SG_GUI_MESH_VIEW_H__

