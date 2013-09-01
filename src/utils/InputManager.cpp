#include "engine/utils/InputManager.h"

#define GLFW_NO_GLU
#define GLFW_INCLUDE_GL3
#include <GL/glfw.h>

void InputManager::keyCallback(int key, int action) {
	InputManager &inputManager = instance();
	if (inputManager.keySubscriptions_.find(key) != inputManager.keySubscriptions_.end())
		inputManager.keySubscriptions_[key](action == GLFW_PRESS);
}
void InputManager::mouseCallback(int button, int action) {
	InputManager &inputManager = instance();
	if (inputManager.mouseSubscriptions_.find(button) != inputManager.mouseSubscriptions_.end())
		inputManager.mouseSubscriptions_[button](action == GLFW_PRESS);
}

void InputManager::mouseWheelCallback(int position) {
	InputManager &inputManager = instance();
	for (auto &callback : inputManager.mouseWheelSubscriptions_) {
		callback.first(position, position - callback.second);
		callback.second = position;
	}
}

InputManager &InputManager::instance() {
	static InputManager inputManager;
	return inputManager;
}

void InputManager::subscribeKey(std::function<void(bool)> callback, int key) {
	keySubscriptions_[key] = callback;
}

void InputManager::subscribeMouse(std::function<void(bool)> callback, int button) {
	mouseSubscriptions_[button] = callback;
}

void InputManager::subscribeMouseWheel(std::function<void(int, int)> callback) {
	glfwSetMouseWheelCallback(mouseWheelCallback);
	mouseWheelSubscriptions_.push_back(std::make_pair(callback, 0));
}

bool InputManager::isKeyPressed(int key) {
	return glfwGetKey(key);
}

InputManager::InputManager() {
	glfwSetKeyCallback(keyCallback);
	glfwSetMouseButtonCallback(mouseCallback);
}