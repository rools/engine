#include "engine/components/PhysicsCharacter.h"

void PhysicsCharacter::setVelocity(const btVector3 &velocity) {
	velocity_ = velocity;
}

btVector3 PhysicsCharacter::getVelocity() {
	return velocity_;
}

ComponentType PhysicsCharacter::type() const {
	return TYPE;
}