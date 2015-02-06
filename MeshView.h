#ifndef SG_GUI_MESH_VIEW_H__
#define SG_GUI_MESH_VIEW_H__

#include <scopegraph/Agent.h>
#include "GuiSignals.h"
#include "ViewSignals.h"
#include "RecordableView.h"
#include "Meshes.h"

namespace sg_gui {

class MeshView :
		public sg::Agent<
			MeshView,
			sg::Accepts<
					Draw,
					QuerySize,
					ChangeAlpha
			>,
			sg::Provides<
					ContentChanged
			>
		>,
		public RecordableView {

public:

	MeshView() : _alpha(1.0) {}

	void setMeshes(std::shared_ptr<Meshes> meshes);

	void onSignal(Draw& signal);

	void onSignal(QuerySize& signal);

	void onSignal(ChangeAlpha& signal);

private:

	void updateRecording();

	std::shared_ptr<Meshes> _meshes;

	double _alpha;
};

} // namespace sg_gui

#endif // SG_GUI_MESH_VIEW_H__

