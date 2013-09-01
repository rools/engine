#include "engine/components/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(CameraType cameraType) :
	cameraType_(cameraType) {
	switch (cameraType) {
		case CAMERA_DEFAULT:
			setPerspective(90.0, 0.1, 1000);
			depthTest_ = true;
			priority_ = 100;
			break;
		case CAMERA_HUD:
			setOrthogonal();
			depthTest_ = false;
			priority_ = 50;
			break;
	}
}

void Camera::setPerspective(float fieldOfViewY, float zNear, float zFar) {
	//projection_ = glm::perspective(fieldOfViewY, 1.0f, near, far);
	projectionType_ = PROJECTION_PERSPECTIVE;
	fieldOfViewY_ = fieldOfViewY;
	zNear_ = zNear;
	zFar_ = zFar;
}

void Camera::setOrthogonal() {
	//projection_ = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -100.0f, 100.0f);
	projectionType_ = PROJECTION_ORHOGONAL;
}

CameraType Camera::cameraType() const {
	return cameraType_;
}

glm::mat4 Camera::projection(int width, int height) const {
	// TODO: Should be replaced when message passing has been implemented
	if (projectionType_ == PROJECTION_PERSPECTIVE) {
		return glm::perspective(fieldOfViewY_, (float)width / height, zNear_, zFar_);
	} else {
		float factor = 0.5 * ((float) width / height);
		return glm::ortho(- factor, factor, -0.5f, 0.5f, -100.0f, 100.0f);
	}
	//return projection_;
}

bool Camera::depthTest() const {
	return depthTest_;
}

int Camera::priority() const {
	return priority_;
}

ComponentType Camera::type() const {
	return TYPE;
}