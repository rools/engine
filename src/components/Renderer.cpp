#include "engine/components/Renderer.h"

Renderer::Renderer(CameraType cameraType, std::string shader) :
	cameraType_(cameraType),
	shader_(shader),
	visible_(true),
	blend_(false),
	overlayed_(false),
	depthTest_(true) {

}

CameraType Renderer::cameraType() const {
	return cameraType_;
}

std::string Renderer::shader() const {
	return shader_;
}

void Renderer::setVisible(bool visible) {
	visible_ = visible;
}

void Renderer::setBlend(bool enabled) {
	blend_ = enabled;
}

void Renderer::setOverlayed(bool enabled) {
	overlayed_ = enabled;
}

void Renderer::setDepthTest(bool enabled) {
	depthTest_ = enabled;
}

bool Renderer::visible() const {
	return visible_;
}

bool Renderer::blend() const {
	return blend_;
}

bool Renderer::overlayed() const {
	return overlayed_;
}

bool Renderer::depthTest() const {
	return depthTest_;
}

ComponentType Renderer::type() const {
	return TYPE;
}