#pragma once

#include <set>

#include "Entity.h"
#include "System.h"

class Game {
public:
	friend Entity;

	void attach(Entity *entity);
	void detach(Entity *entity);

	void attach(System *system);
	// attach(SystemType systemType);

	void run();

	template <class Type>
	Type *get() {
		SystemType t = Type::TYPE;
		auto c = systems_.find(t);

		if (c == systems_.end())
			return nullptr;

		return dynamic_cast<Type *>(c->second);
	}

protected:
	virtual void update(double deltaTime) = 0;

private:
	// Called by attached entities when their types change
	void onTypeChange(Entity *entity, int fromType, int toType);

	std::map<SystemType, System *> systems_;
	std::set<Entity *> entities_;
};