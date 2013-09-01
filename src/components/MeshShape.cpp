#include "engine/components/MeshShape.h"

#include "engine/Entity.h"

ComponentType MeshShape::type() const {
	return TYPE;
}

MeshShape::MeshShape(std::vector<Vertex *> vertices, std::vector<Triangle *> triangles) :
	vertices_(vertices.begin(), vertices.end()),
	triangles_(triangles.begin(), triangles.end()) {

}

MeshShape::~MeshShape() {
	for (Triangle *triangle : triangles_)
		delete triangle;
	for (Vertex *vertex : vertices_)
		delete vertex;
}

std::set<Vertex *> &MeshShape::vertices() {
	return vertices_;
}

std::set<Triangle *> &MeshShape::triangles() {
	return triangles_;
}

void MeshShape::notifyChanged() const {
	sendMessage(MESSAGE_MESH_CHANGED);
}

void MeshShape::addTriangle(Triangle *triangle) {
	triangles_.insert(triangle);

	// Add any vertices that are not already in the mesh
	if (vertices_.find(triangle->vertices[0]) == vertices_.end())
		vertices_.insert(triangle->vertices[0]);
	if (vertices_.find(triangle->vertices[1]) == vertices_.end())
		vertices_.insert(triangle->vertices[1]);
	if (vertices_.find(triangle->vertices[2]) == vertices_.end())
		vertices_.insert(triangle->vertices[2]);

	sendMessage(MESSAGE_MESH_CHANGED);
}

void MeshShape::moveVertex(Vertex *vertex, btVector3 &newPosition) {
	vertex->vector = newPosition;
	sendMessage(MESSAGE_MESH_CHANGED);
}