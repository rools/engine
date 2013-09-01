#pragma once

#include "Camera.h"
#include "../Component.h"

#include <string>

class Renderer : public Component {
public:
	static const ComponentType TYPE = COMPONENT_RENDERER;

	Renderer(CameraType cameraType = CAMERA_DEFAULT, std::string shader = "shaders/deferred_first");

	~Renderer() = default;

	CameraType cameraType() const;
	std::string shader() const;

	void setVisible(bool visible);
	void setBlend(bool enabled);
	void setOverlayed(bool enabled);
	void setDepthTest(bool enabled);

	bool visible() const;
	bool blend() const;
	bool overlayed() const;
	bool depthTest() const;

	ComponentType type() const;

private:
	const CameraType cameraType_;
	const std::string shader_;

	bool visible_;
	bool blend_;
	bool overlayed_;
	bool depthTest_;
};