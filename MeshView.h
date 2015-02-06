#ifndef SG_GUI_MESH_VIEW_H__
#define SG_GUI_MESH_VIEW_H__

#include <scopegraph/Agent.h>
#include <sg_gui/GuiSignals.h>
#include "RecordableView.h"
#include "Meshes.h"

namespace sg_gui {

class MeshView :
		public sg::Agent<
			MeshView,
			sg::Accepts<
					Draw,
					QuerySize
			>
		>,
		public RecordableView {

public:

	void setMeshes(std::shared_ptr<Meshes> meshes);

	void onSignal(Draw& signal);

	void onSignal(QuerySize& signal);

private:

	std::shared_ptr<Meshes> _meshes;
};

} // namespace sg_gui

#endif // SG_GUI_MESH_VIEW_H__

