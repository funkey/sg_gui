#ifndef SG_GUI_SPHERE_H__
#define SG_GUI_SPHERE_H__

#include "Mesh.h"

namespace sg_gui {

class Sphere : public Mesh {

public:

	/**
	 * Construct a unit sphere, centered around the origin. The resolution 
	 * determines the number of lines of longitude per hemisphere.
	 */
	Sphere(int resolution);

private:

	void addOctant(const util::point<float,3>& down, const util::point<float,3>& right);

	int _numLevels;
	int _verticesAdded;
	int _trianglesAdded;
};

} // namespace sg_gui

#endif // SG_GUI_SPHERE_H__

