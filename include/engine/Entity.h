#pragma once

#include "Component.h"
#include "System.h"
#include "MessageTypes.h"

#include <iostream>
#include <map>
#include <set>

class Game;

class Entity {
public:
	friend Game;

	/**
	 * Construct an empty entity.
	 */
	Entity() = default;

	int type() const;

	/**
	 * Attach a component to the entity.
	 *
	 * @param componentType the component to attach
	 */
	void attach(Component *component);

	/**
	 * Detach a component of a given type from the entity.
	 *
	 * @param componentType the type of the component to detach
	 */
	void detach(ComponentType componentType);

	/**
	 * Check if the entity contains a component of a given type.
	 *
	 * @param componentType the type of the component
	 * @return whether a component of the given type exists
	 */
	bool has(ComponentType componentType) const;

	/**
	 * Get an attached component.
	 *
	 * @tparam Type component class
	 * @return the component if the entity contains one of the given type, otherwise nullptr
	 */
	template <class Type>
	Type *get() {
		ComponentType t = Type::TYPE;
		auto c = components_.find(t);

		if (c == components_.end())
			return nullptr;

		return dynamic_cast<Type *>(c->second);
	}

	template <class Type>
	Type *get() const {
		ComponentType t = Type::TYPE;
		auto c = components_.find(t);

		if (c == components_.end())
			return nullptr;

		return dynamic_cast<Type *>(c->second);
	}

	void sendMessage(MessageType messageType);

	void subscribe(MessageType messageType, System *system);
	void unsubscribe(MessageType messageType, System *system);

private:
	void attach(Game *game);
	void detachGame();

	std::map<ComponentType, Component *> components_;
	int type_ = 0;
	Game *game_ = nullptr;

	std::map<MessageType, std::set<System *>> subscribers_;
};