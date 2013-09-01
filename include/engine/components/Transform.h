#pragma once

#include "../Component.h"

#include <bullet/btBulletDynamicsCommon.h>

class Transform : public Component {
public:
	static const ComponentType TYPE = COMPONENT_TRANSFORM;

	Transform();
	Transform(const btTransform &transform);
	Transform(const btVector3 &position);
	Transform(float x, float y, float z);

	~Transform() = default;

	btTransform transform() const;
	void setTransform(const btTransform &transform);
	void setRotation(const btQuaternion &rotation);

	ComponentType type() const;

private:
	btTransform transform_;
};