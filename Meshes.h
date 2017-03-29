#ifndef SG_GUI_MESHES_H__
#define SG_GUI_MESHES_H__

#include <imageprocessing/Volume.h>
#include <util/Lockable.h>
#include "Mesh.h"

namespace sg_gui {

class Meshes : public Volume, public Lockable {

public:

	Meshes() {}

	void add(unsigned int id, std::shared_ptr<Mesh> mesh, int color = -1) {

		_meshes[id] = mesh;
		_colors[id] = (color < 0 ? id : color);

		setBoundingBoxDirty();
	}

	void remove(unsigned int id) {

		_meshes.erase(id);
		_colors.erase(id);

		setBoundingBoxDirty();
	}

	std::shared_ptr<Mesh> get(unsigned int id) {

		if (_meshes.count(id))
			return _meshes[id];

		return std::shared_ptr<Mesh>();
	}

	int getColor(unsigned int id) {

		return _colors[id];
	}

	const std::vector<unsigned int> getMeshIds() const {

		std::vector<unsigned int> ids;
		for (auto& p : _meshes)
			ids.push_back(p.first);
		return ids;
	}

	void clear() { _meshes.clear(); _colors.clear(); setBoundingBoxDirty(); }

	bool contains(unsigned int id) const { return _meshes.count(id); }

private:

	util::box<float,3> computeBoundingBox() const {

		util::box<float,3> boundingBox;

		std::map<unsigned int, std::shared_ptr<Mesh> >::const_iterator i;
		for (i = _meshes.begin(); i != _meshes.end(); i++)
			boundingBox += i->second->getBoundingBox();

		return boundingBox;
	}

	std::map<unsigned int, std::shared_ptr<Mesh> > _meshes;
	std::map<unsigned int, int >                   _colors;
};

} // namespace sg_gui

#endif // SG_GUI_MESHES_H__

