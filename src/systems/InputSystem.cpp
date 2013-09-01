#include "engine/systems/InputSystem.h"

#include "engine/Entity.h"

InputSystem::InputSystem() {
	//glfwGetMousePos(&lastX_, &lastY_);
}

void InputSystem::update(double deltaTime) {
	const float MOUSE_SENSITIVITY = 0.02;

	int mouseX = 0;
	int mouseY = 0;

	glfwGetMousePos(&mouseX, &mouseY);

	float deltaX = mouseX - lastX_;
	float deltaY = mouseY - lastY_;

	lastX_ = mouseX;
	lastY_ = mouseY;

	yaw_ -= deltaX * MOUSE_SENSITIVITY;
	pitch_ = btClamped(pitch_ - deltaY * MOUSE_SENSITIVITY, (float)-M_PI_2, (float)M_PI_2);

	for (Entity *entity : inputControllers_) {
		const float speed = 0.10f;
		btVector3 velocity(0.0f, 0.0f, 0.0f);
		if (glfwGetKey('W'))
			velocity += btVector3(0.0f, 0.0f, -1.0);
		if (glfwGetKey('S'))
			velocity += btVector3(0.0f, 0.0f, 1.0);
		if (glfwGetKey('A'))
			velocity += btVector3(-1.0, 0.0f, 0.0f);
		if (glfwGetKey('D'))
			velocity += btVector3(1.0, 0.0f, 0.0f);
		if (velocity.length2() > 1)
			velocity.normalize();
		velocity *= speed;

		velocity = btTransform(entity->get<Transform>()->transform().getRotation()) * velocity;
		entity->get<PhysicsCharacter>()->setVelocity(velocity);

		entity->get<Transform>()->setRotation(btQuaternion(yaw_, pitch_, 0.0));
	}
}

SystemType InputSystem::type() const {
	return TYPE;
}

void InputSystem::onChangeType(Entity *entity, int fromType, int toType) {
	const bool fromInputController = (fromType & TYPE_INPUT_CONTROLLER) == TYPE_INPUT_CONTROLLER;
	const bool toInputController = (toType & TYPE_INPUT_CONTROLLER) == TYPE_INPUT_CONTROLLER;

	if (!fromInputController && toInputController) {
		// An input controller was attached
		inputControllers_.insert(entity);
	} else if (fromInputController && ! toInputController) {
		// An input controller was detached
		inputControllers_.erase(entity);
	}
}

void InputSystem::onMessage(MessageType messageType, Entity *entity) {

}

void InputSystem::onAttach(Game *game) {

}