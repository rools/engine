#include "engine/Game.h"

#include <GL/glfw.h>

void Game::attach(Entity *entity) {
	for (auto system : systems_)
		system.second->onChangeType(entity, 0, entity->type());
	entity->attach(this);
	entities_.insert(entity);
}

void Game::detach(Entity *entity) {
	for (auto system : systems_)
		system.second->onChangeType(entity, entity->type(), 0);
	entity->detachGame();
	entities_.erase(entity);
}

void Game::attach(System *system) {
	system->onAttach(this);
	systems_[system->type()] = system;
	for (auto entity : entities_)
		system->onChangeType(entity, 0, entity->type());
}

void Game::run() {
	double lastTime = glfwGetTime();

	int frameCount = 0;
	float elapsedTime = 0.0;

	while (true) {
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		for (auto system : systems_)
			system.second->update(deltaTime);

		update(deltaTime);

		elapsedTime += deltaTime;
		frameCount++;
		if (elapsedTime >= 1.0) {
			std::cout << frameCount / elapsedTime << " fps" << std::endl;
			elapsedTime = 0.0;
			frameCount = 0;
		}

		//glfwSleep(1.0 / 60.0);
	}
}

void Game::onTypeChange(Entity *entity, int fromType, int toType) {
	for (auto system : systems_)
		system.second->onChangeType(entity, fromType, toType);
}