#pragma once

#include <functional>
#include <map>
#include <vector>

class InputManager {
public:
	static InputManager &instance();

	void subscribeKey(std::function<void(bool)> callback, int key);
	void subscribeMouse(std::function<void(bool)> callback, int button);
	void subscribeMouseWheel(std::function<void(int, int)> callback);
	bool isKeyPressed(int key);

private:
	// TODO: Maybe allow more than one subscriber per key/button?
	std::map<int, std::function<void(bool)>> keySubscriptions_;
	std::map<int, std::function<void(bool)>> mouseSubscriptions_;

	std::vector<std::pair<std::function<void(int, int)>, int>> mouseWheelSubscriptions_;

	static void  keyCallback(int key, int action);
	static void  mouseCallback(int button, int action);
	static void  mouseWheelCallback(int position);

	InputManager();
};