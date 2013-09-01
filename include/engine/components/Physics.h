#pragma once

#include "../Component.h"

#include <bullet/btBulletDynamicsCommon.h>

enum PhysicsType {
	PHYSICS_STATIC  = 1,
	PHYSICS_DYNAMIC = 2,
};

class Physics : public Component {
public:
	static const ComponentType TYPE = COMPONENT_PHYSICS;

	Physics(PhysicsType physicsType);

	~Physics() = default;

	PhysicsType physicsType() const;

	ComponentType type() const;

private:
	const PhysicsType physicsType_;

	btVector3 constantVelocity_;
};