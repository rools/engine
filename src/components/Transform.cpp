#include "engine/components/Transform.h"

Transform::Transform() :
	transform_(btQuaternion(0.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f)) {

}

Transform::Transform(const btTransform &transform) :
	transform_(transform) {

}

Transform::Transform(const btVector3 &position) :
	transform_(btQuaternion(0.0, 0.0, 0.0), position) {

}

Transform::Transform(float x, float y, float z) :
	transform_(btQuaternion(0.0, 0.0, 0.0), btVector3(x, y, z)) {

}

btTransform Transform::transform() const {
	return transform_;
}

void Transform::setTransform(const btTransform &transform) {
	transform_ = transform;
}

void Transform::setRotation(const btQuaternion &rotation) {
	transform_.setRotation(rotation);
}

ComponentType Transform::type() const {
	return TYPE;
}