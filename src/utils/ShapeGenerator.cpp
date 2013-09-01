#include "engine/utils/ShapeGenerator.h"

#include <vector>

MeshShape *ShapeGenerator::plane(Color color, float width, float height) {
	std::vector<Vertex *> vertices;
	for (int i = 0; i < 4; ++i)
		vertices.push_back(new Vertex);

	vertices[0]->vector.setX(-width * 0.5f);
	vertices[0]->vector.setY(0.0f);
	vertices[0]->vector.setZ(-height * 0.5f);

	vertices[1]->vector.setX(-width * 0.5f);
	vertices[1]->vector.setY(0.0f);
	vertices[1]->vector.setZ(height * 0.5f);

	vertices[2]->vector.setX(width * 0.5f);
	vertices[2]->vector.setY(0.0f);
	vertices[2]->vector.setZ(height * 0.5f);

	vertices[3]->vector.setX(width * 0.5f);
	vertices[3]->vector.setY(0.0f);
	vertices[3]->vector.setZ(-height * 0.5f);

	std::vector<Triangle *> triangles;
	for (int i = 0; i < 2; ++i)
		triangles.push_back(new Triangle);

	triangles[0]->color = color;
	triangles[0]->vertices[0] = vertices[0];
	triangles[0]->vertices[1] = vertices[1];
	triangles[0]->vertices[2] = vertices[2];

	triangles[1]->color = color;
	triangles[1]->vertices[0] = vertices[0];
	triangles[1]->vertices[1] = vertices[2];
	triangles[1]->vertices[2] = vertices[3];

	return new MeshShape(vertices, triangles);
}

MeshShape *ShapeGenerator::cylinder(Color color, float radius, float height) {
	const int subdivs = 10;

	std::vector<Vertex *> vertices;
	std::vector<Triangle *> triangles;

	vertices.push_back(new Vertex());
	vertices.push_back(new Vertex());

	vertices[0]->vector = btVector3(0, 0.5 * height, 0);
	vertices[1]->vector = btVector3(0, - 0.5 * height, 0);

	//Vertex *circles = new Vertex[subdivs * 2];
	for (int i = 0; i < subdivs * 2; ++i)
		vertices.push_back(new Vertex());

	for (int i = 0; i < subdivs; ++i) {
		vertices[i + 2]->vector = btVector3(radius * cos(2.0 * M_PI * ((float)i / subdivs)), 0.5 * height, radius * sin(2.0 * M_PI * ((float)i / subdivs)));
		vertices[i + subdivs + 2]->vector = vertices[i + 2]->vector - btVector3(0.0, height, 0.0);

		Triangle *triangle1 = new Triangle();
		triangle1->color = color;
		triangle1->vertices[0] = vertices[0];
		triangle1->vertices[1] = vertices[i + 2];
		triangle1->vertices[2] = vertices[(i + 1) % subdivs + 2];
		triangles.push_back(triangle1);

		Triangle *triangle2 = new Triangle();
		triangle2->color = color;
		triangle2->vertices[0] = vertices[1];
		triangle2->vertices[1] = vertices[i + subdivs + 2];
		triangle2->vertices[2] = vertices[(i + 1) % subdivs + subdivs + 2];
		triangles.push_back(triangle2);

		Triangle *triangle3 = new Triangle();
		triangle3->color = color;
		triangle3->vertices[0] = vertices[i + 2];
		triangle3->vertices[1] = vertices[(i + 1) % subdivs + 2];
		triangle3->vertices[2] = vertices[i + subdivs + 2];
		triangles.push_back(triangle3);

		Triangle *triangle4 = new Triangle();
		triangle4->color = color;
		triangle4->vertices[0] = vertices[(i + 1) % subdivs + 2];
		triangle4->vertices[1] = vertices[i + subdivs + 2];
		triangle4->vertices[2] = vertices[(i + 1) % subdivs + subdivs + 2];
		triangles.push_back(triangle4);
	}

	return new MeshShape(vertices, triangles);
}