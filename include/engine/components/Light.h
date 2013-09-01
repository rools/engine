#pragma once

#include "../common.h"
#include "../Component.h"

enum LightType {
	LIGHT_POINT,
};

class Light : public Component {
public:
	static const ComponentType TYPE = COMPONENT_LIGHT;

	ComponentType type() const;

	Light(LightType lightType = LIGHT_POINT, Color color = Color(255, 255, 255), float range = 5.0);

	~Light() = default;

	LightType lightType() const;
	Color color() const;
	float range() const;

private:
	LightType lightType_;
	Color color_;
	float range_;
};