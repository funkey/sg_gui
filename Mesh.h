#ifndef SG_GUI_MESH_H__
#define SG_GUI_MESH_H__

#include <vector>
#include <limits>
#include <imageprocessing/Volume.h>
#include <util/foreach.h>
#include "Point3d.h"
#include "Vector3d.h"
#include "Triangle.h"

namespace sg_gui {

/**
 * A 3D mesh as a list of triangles.
 */
class Mesh : public Volume {

public:

	Mesh() {}

	/**
	 * Set the number of vertices (and normals) to allocate for this mesh.
	 */
	void setNumVertices(unsigned int numVertices)   { _vertices.resize(numVertices); _normals.resize(numVertices); }

	/**
	 * Set the number of triangles to allocate for this mesh.
	 */
	void setNumTriangles(unsigned int numTriangles) { _triangles.resize(numTriangles); }

	/**
	 * Number of vertices (and normals) of this mesh.
	 */
	unsigned int getNumVertices()   { return _vertices.size(); }

	/**
	 * The number of triangles that constitute this mesh.
	 */
	unsigned int getNumTriangles() { return _triangles.size(); }

	/**
	 * Set a vertex by index.
	 */
	void setVertex(unsigned int index, const Point3d&  vertex) { _vertices[index] = vertex; setBoundingBoxDirty(); }

	/**
	 * Set a vertex' normal by index.
	 */
	void setNormal(unsigned int index, const Vector3d& normal) { _normals[index]  = normal; }

	/**
	 * Set a triangle by specifying three vertices by index.
	 */
	void setTriangle(
			unsigned int index,
			unsigned int v1,
			unsigned int v2,
			unsigned int v3) {

		_triangles[index] = Triangle(v1, v2, v3);
	}

	/**
	 * Get a vertex by index.
	 */
	Point3d&  getVertex(unsigned int index) { return _vertices[index]; }
	const Point3d&  getVertex(unsigned int index) const { return _vertices[index]; }

	/**
	 * Get a vertex' normal by index.
	 */
	Vector3d& getNormal(unsigned int index) { return _normals[index];  }
	const Vector3d& getNormal(unsigned int index) const { return _normals[index];  }

	/**
	 * Get a triangle of this mesh by index.
	 */
	Triangle& getTriangle(unsigned int index) { return _triangles[index]; }
	const Triangle& getTriangle(unsigned int index) const { return _triangles[index]; }

	/**
	 * Get a vector of all vertices of this mesh.
	 */
	const std::vector<Point3d>&  getVertices()  const { return _vertices; }

	/**
	 * Get a vector of all normals of this mesh.
	 */
	const std::vector<Vector3d>& getNormals()   const { return _normals; }

	/**
	 * Get a vector of all triangles that constitute this mesh.
	 */
	const std::vector<Triangle>& getTriangles() const { return _triangles; }

	/**
	 * Create a submesh from a selection of triangles of this mesh.
	 *
	 * @param triangles
	 *              Indices of the triangles to use in the submesh.
	 *
	 * @return A mesh containing only the specified triangles.
	 */
	Mesh createSubmesh(const std::vector<unsigned int>& triangles);

private:

	util::box<float,3> computeBoundingBox() const {

		util::box<float,3> bb;

		for (const Point3d& p: _vertices)
			bb.fit(util::point<float,3>(p.x, p.y, p.z));

		return bb;
	}

	/**
	 * Remove all vertices (and normals) that are not used by any triangle.
	 */
	void strip();

	// the vertices of the mesh
	std::vector<Point3d>  _vertices;

	// the normals, one for each vertex
	std::vector<Vector3d> _normals;

	// list of triangles that make up the mesh
	std::vector<Triangle> _triangles;
};

} // namespace sg_gui

#endif // SG_GUI_MESH_H__

