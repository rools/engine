#include "engine/utils/MeshLoader.h"

#include <fstream>
#include <vector>

MeshShape *MeshLoader::load(const std::string &fileName, Color defaultColor) {
	std::ifstream file(fileName);

	if (!file.is_open())
		throw std::runtime_error("Could not open model " + fileName);

	// In .obj files, triangles are defined with index references to vertices
	struct IndexTriangle {int vertices[3];};

	std::vector<Vertex *> vertices;
	std::vector<IndexTriangle> indexTriangles;

	while (!file.eof()) {
		std::string type;
		file >> type;

		if (type == "")
			break;

		if (type == "v") {
			// Vertex
			Vertex *vertex = new Vertex();
			float x, y, z;
			file >> x >> y >> z;
			vertex->vector.setX(x);
			vertex->vector.setY(y);
			vertex->vector.setZ(z);
			vertices.push_back(vertex);
		} else if (type == "f") {
			// Face
			IndexTriangle triangle;
			file >> triangle.vertices[0] >> triangle.vertices[1] >> triangle.vertices[2];
			indexTriangles.push_back(triangle);
		} else if (type[0] == '#') {
			// Comment
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		} else if (type == "s") {
			// Smooth shading
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		} else if (type == "mtllib") {
			// Material file
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		} else if (type == "usemtl") {
			// Use material
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		} else {
			throw std::runtime_error("Error parsing .obj file (" + type + ")");
		}

		if (file.bad())
			throw std::runtime_error("Error parsing .obj file");
	}

	std::vector<Triangle *> triangles;
	for (IndexTriangle &indexTrignale : indexTriangles) {
		Triangle *triangle = new Triangle();
		triangle->vertices[0] = vertices[indexTrignale.vertices[0] - 1];
		triangle->vertices[1] = vertices[indexTrignale.vertices[1] - 1];
		triangle->vertices[2] = vertices[indexTrignale.vertices[2] - 1];
		triangle->color = defaultColor;
		triangles.push_back(triangle);
	}
	return new MeshShape(vertices, triangles);
}