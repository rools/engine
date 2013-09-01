#pragma once

#include "MessageTypes.h"

#include <set>

enum ComponentType {
	COMPONENT_TRANSFORM         = 1 << 0,
	COMPONENT_CAMERA            = 1 << 1,
	COMPONENT_MESH_SHAPE        = 1 << 2,
	COMPONENT_RENDERER          = 1 << 3,
	COMPONENT_LIGHT             = 1 << 4,
	COMPONENT_PHYSICS           = 1 << 5,
	COMPONENT_PHYSICS_CHARACTER = 1 << 6,
	COMPONENT_INPUT_CONTROLLER  = 1 << 7,
};

class Entity;

class Component {
public:
	friend class Entity;
	/**
	 * Get the type of this component
	 *
	 * @return the type of this component
	 */
	virtual ComponentType type() const = 0;

	virtual ~Component() = 0;

protected:
	//static int COUNT;
	//int id_ = -1;

	void sendMessage(MessageType messageType) const;

private:
	std::set<Entity *> entities_;
};