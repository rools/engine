#include "engine/components/Physics.h"

Physics::Physics(PhysicsType physicsType) :
	physicsType_(physicsType) {

}

PhysicsType Physics::physicsType() const {
	return physicsType_;
}

ComponentType Physics::type() const {
	return TYPE;
}