#ifndef SG_GUI_MESHES_H__
#define SG_GUI_MESHES_H__

#include <util/Volume.h>
#include "Mesh.h"

namespace sg_gui {

class Meshes : public Volume {

public:

	Meshes() {}

	void add(unsigned int id, boost::shared_ptr<Mesh> mesh) {

		_meshes[id] = mesh;
		_ids.push_back(id);

		setBoundingBoxDirty();
	}

	boost::shared_ptr<Mesh> get(unsigned int id) {

		if (_meshes.count(id))
			return _meshes[id];

		return boost::shared_ptr<Mesh>();
	}

	const std::vector<unsigned int>& getMeshIds() const {

		return _ids;
	}

	void clear() { _meshes.clear(); _ids.clear(); resetBoundingBox(); }

private:

	BoundingBox computeBoundingBox() const {

		BoundingBox boundingBox;

		std::map<unsigned int, boost::shared_ptr<Mesh> >::const_iterator i;
		for (i = _meshes.begin(); i != _meshes.end(); i++)
			boundingBox += i->second->getBoundingBox();

		return boundingBox;
	}

	std::map<unsigned int, boost::shared_ptr<Mesh> > _meshes;

	std::vector<unsigned int> _ids;
};

} // namespace sg_gui

#endif // SG_GUI_MESHES_H__

