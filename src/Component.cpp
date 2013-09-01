#include "engine/Component.h"

#include "engine/Entity.h"

#include <cassert>

Component::~Component() {

}

void Component::sendMessage(MessageType messageType) const {
	for (Entity *entity : entities_)
		entity->sendMessage(messageType);
}