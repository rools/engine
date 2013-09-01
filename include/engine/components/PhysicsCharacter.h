#pragma once

#include "../Component.h"

#include <bullet/btBulletDynamicsCommon.h>

class PhysicsCharacter : public Component {
public:
	static const ComponentType TYPE = COMPONENT_PHYSICS_CHARACTER;

	~PhysicsCharacter() = default;

	void setVelocity(const btVector3 &velocity);
	btVector3 getVelocity();

	ComponentType type() const;

private:
	btVector3 velocity_;
};