#pragma once

#include "../components/MeshShape.h"

class MeshLoader {
public:
	static MeshShape *load(const std::string &fileName, Color defaultColor = Color(255, 255, 255));
};