#pragma once

#include "../components/InputController.h"
#include "../components/PhysicsCharacter.h"
#include "../components/Transform.h"
#include "../System.h"

#define GLFW_NO_GLU
#define GLFW_INCLUDE_GL3
#include <GL/glfw.h>

#include <set>

class InputSystem : public System {
public:
	static const SystemType TYPE = SYSTEM_INPUT;

	InputSystem();

	void update(double deltaTime);

	SystemType type() const;

private:
	static const int TYPE_INPUT_CONTROLLER = InputController::TYPE | PhysicsCharacter::TYPE | Transform::TYPE;

	int lastX_ = 0.0;
	int lastY_ = 0.0;

	float yaw_ = 0.0;
	float pitch_ = 0.0;

	std::set<Entity *> inputControllers_;

	void onChangeType(Entity *entity, int fromType, int toType);

	void onMessage(MessageType messageType, Entity *entity);

	void onAttach(Game *game);
};