#pragma once

#include "MessageTypes.h"

enum SystemType {
	SYSTEM_PHYSICS,
	SYSTEM_RENDER,
	SYSTEM_INPUT,

	SYSTEM_TERRAIN,
	SYSTEM_OCTREE_EDITOR,
};

class Entity;
class Game;

class System {
public:
	friend Game;
	friend Entity;

	virtual SystemType type() const = 0;

protected:
	/**
	 * Called when a new entity of the associated type exists in the game.
	 *
	 * @param entity the entity that was attached
	 */
	//virtual void onAttach(Entity *Entity) = 0;

	/**
	 * Called when a previously associated entity has been detached or now is of the wrong type.
	 *
	 * @param entity the entity that was detached
	 */
	//virtual void onDetach(Entity *Entity) = 0;

	virtual void onChangeType(Entity *entity, int fromType, int toType) = 0;

	virtual void onMessage(MessageType messageType, Entity *entity) = 0;

	virtual void onAttach(Game *game) = 0;

	/**
	 * Called when another component is attached to the same game.
	 *
	 * @param componentSystem the component system that was attached
	 */
	//virtual void onAttach(ComponentSystem *componentSystem);

	/**
	 * Called when another component is detached from the same game.
	 *
	 * @param componentSystem the component system that was detached
	 */
	//virtual void onDetach(ComponentSystem *componentSystem);

private:

	virtual void update(double deltaTime) = 0;
};