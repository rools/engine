#pragma once

#include "../components/MeshShape.h"

class ShapeGenerator {
public:
	static MeshShape *plane(Color color, float width, float height);
	static MeshShape *cylinder(Color color, float radius, float height);
};