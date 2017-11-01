#ifndef SG_GUI_MESH_VIEW_H__
#define SG_GUI_MESH_VIEW_H__

#include <scopegraph/Agent.h>
#include <imageprocessing/ExplicitVolume.h>
#include "GuiSignals.h"
#include "SegmentSignals.h"
#include "ViewSignals.h"
#include "KeySignals.h"
#include "RecordableView.h"
#include "Meshes.h"
#include <future>
#include <thread>

namespace sg_gui {

/**
 * A marching cubes adaptor that binarizes an explicit volume by reporting 1 for 
 * a given label and 0 otherwise.
 */
template <typename EV>
class ExplicitVolumeLabelAdaptor {

public:

	typedef typename EV::value_type value_type;

	ExplicitVolumeLabelAdaptor(const EV& ev, value_type label) :
		_ev(ev),
		_label(label) {}

	const util::box<float,3>& getBoundingBox() const { return _ev.getBoundingBox(); }

	float operator()(float x, float y, float z) const {

		if (!getBoundingBox().contains(x, y, z))
			return 0;

		unsigned int dx, dy, dz;

		_ev.getDiscreteCoordinates(x, y, z, dx, dy, dz);

		return (_ev(dx, dy, dz) == _label);
	}

private:

	const EV& _ev;

	value_type _label;
};

class MeshView :
		public sg::Agent<
			MeshView,
			sg::Accepts<
					ShowSegment,
					HideSegment,
					DrawOpaque,
					DrawTranslucent,
					QuerySize,
					ChangeAlpha,
					SetAlphaPlane,
					KeyDown
			>,
			sg::Provides<
					ContentChanged
			>
		>,
		public RecordableView {

public:

	MeshView(std::shared_ptr<ExplicitVolume<uint64_t>> labels);

	void setOffset(util::point<float, 3> offset);

	void onSignal(DrawOpaque& signal);

	void onSignal(DrawTranslucent& signal);

	void onSignal(QuerySize& signal);

	void onSignal(ChangeAlpha& signal);

	void onSignal(SetAlphaPlane& signal);

	void onSignal(ShowSegment& signal);

	void onSignal(HideSegment& signal);

	void onSignal(KeyDown& signal);

private:

	void notifyMeshExtracted(std::shared_ptr<sg_gui::Mesh> mesh, uint64_t label);

	void exportMeshes();

	void updateRecording();

	void setVertexAlpha(const Point3d& p, float r, float g, float b);

	std::shared_ptr<ExplicitVolume<uint64_t>> _labels;

	std::shared_ptr<Meshes> _meshes;

	std::map<uint64_t, std::shared_ptr<sg_gui::Mesh>> _meshCache;

	std::vector<std::future<std::shared_ptr<sg_gui::Mesh>>> _highresMeshFutures;

	float _minCubeSize;

	double _alpha;
	util::plane<float, 3> _alphaPlane;
	double _alphaFalloff;
	bool _haveAlphaPlane;

	util::point<float, 3> _offset;

	unsigned int _numThreads;
	unsigned int _maxNumThreads;
};

} // namespace sg_gui

#endif // SG_GUI_MESH_VIEW_H__

