#include <util/geometry.hpp>
#include <util/assert.h>
#include <cmath>
#include "Sphere.h"

namespace sg_gui {

Sphere::Sphere(int numLevels) :
	_numLevels(numLevels) {

	int numVertices  = 8*(_numLevels + 1)*(_numLevels + 2)/2;
	int numTriangles = 8*(_numLevels*(_numLevels + 1) - _numLevels);

	setNumVertices(numVertices);
	setNumTriangles(numTriangles);

	_verticesAdded  = 0;
	_trianglesAdded = 0;

	addOctant(
			util::point<float,3>(0, 0, 1) - util::point<float,3>(0, 1, 0),
			util::point<float,3>(1, 0, 0) - util::point<float,3>(0, 0, 1));
	addOctant(
			util::point<float,3>(1, 0,  0) - util::point<float,3>(0, 1, 0),
			util::point<float,3>(0, 0, -1) - util::point<float,3>(1, 0, 0));
	addOctant(
			util::point<float,3>( 0, 0, -1) - util::point<float,3>(0, 1,  0),
			util::point<float,3>(-1, 0,  0) - util::point<float,3>(0, 0, -1));
	addOctant(
			util::point<float,3>(-1, 0, 0) - util::point<float,3>( 0, 1, 0),
			util::point<float,3>( 0, 0, 1) - util::point<float,3>(-1, 0, 0));

	addOctant(
			util::point<float,3>(0, 0, 1) - util::point<float,3>(0, -1, 0),
			util::point<float,3>(1, 0, 0) - util::point<float,3>(0,  0, 1));
	addOctant(
			util::point<float,3>(1, 0,  0) - util::point<float,3>(0, -1, 0),
			util::point<float,3>(0, 0, -1) - util::point<float,3>(1,  0, 0));
	addOctant(
			util::point<float,3>( 0, 0, -1) - util::point<float,3>(0, -1,  0),
			util::point<float,3>(-1, 0,  0) - util::point<float,3>(0,  0, -1));
	addOctant(
			util::point<float,3>(-1, 0, 0) - util::point<float,3>( 0, -1, 0),
			util::point<float,3>( 0, 0, 1) - util::point<float,3>(-1,  0, 0));

	UTIL_ASSERT_REL(_verticesAdded,  ==, numVertices);
	UTIL_ASSERT_REL(_trianglesAdded, ==, numTriangles);
}

void
Sphere::addOctant(const util::point<float,3>& down, const util::point<float,3>& right) {

	int indexOffset = _verticesAdded;

	// first pass: create vertices

	for (int l = 0; l <= _numLevels; l++) {

		int numLevelPoints = l + 1;

		// how much to go down on the triangle
		float d = (float)l/_numLevels;

		for (int p = 0; p < numLevelPoints; p++) {

			// how much to go right on the triangle
			float r = (numLevelPoints == 1 ? 0 : (float)p/(numLevelPoints - 1));

			// postion of point i on the triangle
			util::point<float,3> pos = util::point<float,3>(0, -down.y(), 0) + d*down + d*r*right;

			// normalize to make a sphere
			pos /= sqrt(dot(pos, pos));
			setVertex(_verticesAdded, pos);
			setNormal(_verticesAdded, pos);
			_verticesAdded++;
		}
	}

	// second pass: find triangles

	for (int l = 1; l <= _numLevels; l++) {

		int numLevelPoints = l + 1;
		int numPreviousLevelPoints = l;
		int firstPointIndex = l*(l + 1)/2;

		// for each edge (u,v) in this level
		for (int u = 0; u < numLevelPoints - 1; u++) {

			// global indices
			int i =  u      + firstPointIndex + indexOffset;
			int j = (u + 1) + firstPointIndex + indexOffset;

			// index of vertex above edge
			int above = i - numPreviousLevelPoints;

			setTriangle(_trianglesAdded, i, j, above);
			_trianglesAdded++;

			// index of vertex below edge
			int below = i + numLevelPoints + 1;

			if (below < _verticesAdded) {

				setTriangle(_trianglesAdded, i, j, below);
				_trianglesAdded++;
			}
		}
	}
}

} // namespace sg_gui
