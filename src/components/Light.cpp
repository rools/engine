#include "engine/components/Light.h"

ComponentType Light::type() const {
	return TYPE;
}

Light::Light(LightType lightType, Color color, float range) :
	lightType_(lightType),
	color_(color),
	range_(range) {

}

LightType Light::lightType() const {
	return lightType_;
}

Color Light::color() const {
	return color_;
}

float Light::range() const {
	return range_;
}