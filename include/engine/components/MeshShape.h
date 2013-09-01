#pragma once

#include "../common.h"
#include "../Component.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <set>
#include <vector>

struct Vertex {
	btVector3 vector;
};

struct Triangle {
	Vertex *vertices[3];
	Color color;
};

class MeshShape : public Component {
public:
	static const ComponentType TYPE = COMPONENT_MESH_SHAPE;

	ComponentType type() const;

	MeshShape() = default;
	MeshShape(std::vector<Vertex *> vertices, std::vector<Triangle *> triangles);
	~MeshShape();

	std::set<Vertex *> &vertices();
	std::set<Triangle *> &triangles();

	void notifyChanged() const;

	void addTriangle(Triangle *triangle);
	void moveVertex(Vertex *vertex, btVector3 &newPosition);

protected:
	std::set<Vertex *> vertices_;
	std::set<Triangle *> triangles_;
};