#pragma once

#include "../Component.h"

#include <glm/glm.hpp>

enum CameraType {
	CAMERA_DEFAULT = 1,
	CAMERA_HUD     = 2,
};

class Camera : public Component {
public:
	static const ComponentType TYPE = COMPONENT_CAMERA;

	Camera(CameraType cameraType);

	~Camera() = default;

	void setPerspective(float fieldOfViewY, float zNear, float zFar);
	void setOrthogonal();

	CameraType cameraType() const;
	glm::mat4 projection(int width, int height) const;
	bool depthTest() const;
	int priority() const;

	ComponentType type() const;

private:
	enum ProjectionType {
		PROJECTION_PERSPECTIVE,
		PROJECTION_ORHOGONAL
	};

	const CameraType cameraType_;

	// Variables needed to generate projection matrix
	ProjectionType projectionType_;
	float fieldOfViewY_;
	float zNear_;
	float zFar_;

	// The projection matrix of this camera
	//glm::mat4 projection_;

	// Whether the depth buffer should be considered
	bool depthTest_;

	// Render priority. Higher priorities are rendered first.
	int priority_;
};