#pragma once

#include "../Component.h"

class InputController : public Component {
public:
	static const ComponentType TYPE = COMPONENT_INPUT_CONTROLLER;

	~InputController() = default;

	ComponentType type() const;
};