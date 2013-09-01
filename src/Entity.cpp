#include "engine/Entity.h"

#include "engine/Game.h"

#include <cassert>

int Entity::type() const {
	return type_;
}

void Entity::attach(Component *component) {
	assert(component != nullptr);
	assert(this != nullptr);
	component->entities_.insert(this);
	components_[component->type()] = component;
	int oldType = type_;
	type_ |= component->type();
	if (game_)
		game_->onTypeChange(this, oldType, type_);
}

void Entity::detach(ComponentType componentType) {
	auto component = components_.find(componentType);
	component->second->entities_.erase(this);
	components_.erase(component);
	int oldType = type_;
	type_ &= ~componentType;
	if (game_)
		game_->onTypeChange(this, oldType, type_);
}

bool Entity::has(ComponentType componentType) const {
	return components_.find(componentType) != components_.end();
}

void Entity::subscribe(MessageType messageType, System *system) {
	subscribers_[messageType].insert(system);
}

void Entity::unsubscribe(MessageType messageType, System *system) {
	subscribers_[messageType].erase(system);
}

void Entity::attach(Game *game) {
	game_ = game;
}

void Entity::detachGame() {
	game_ = nullptr;
}

void Entity::sendMessage(MessageType messageType) {
	for (System *system : subscribers_[messageType])
		system->onMessage(messageType, this);
}