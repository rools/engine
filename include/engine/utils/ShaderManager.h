#pragma once

#include "../Components/Light.h"

#define GLFW_NO_GLU
#define GLFW_INCLUDE_GL3
#include <GL/glfw.h>

#include <bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <map>
#include <string>

class ShaderManager {
public:
	static ShaderManager &getInstance();
	~ShaderManager();

	void loadShader(const std::string &name);

	void setModelMatrix(const glm::mat4 &matrix);
	void setModelMatrix(const btTransform &transform);
	void setViewMatrix(const glm::mat4 &matrix);
	void setViewMatrix(const btTransform &transform);
	void setProjectionMatrix(const glm::mat4 &matrix);

	// Set optional uniforms
	void setResolution(int width, int height);

	void setCamera(const Entity *entiy);
	void setLight(const Entity *entity);

	void validateCurrentShader();

private:
	struct Shader {
		GLuint program;
		GLuint vertexShader;
		GLuint geometryShader;
		GLuint fragmentShader;

		// OpenGL uniforms
		GLint modelMatrix;
		GLint viewMatrix;
		GLint projectionMatrix;

		// Optional shader uniforms
		GLint resolution;
		GLint cameraPosition;
		GLint lightPosition;
		GLint lightColor;
		GLint lightRange;
	};

	ShaderManager() = default;
	static GLuint compileShader(const std::string &fileName, GLenum type, bool optional);

	Shader loadedShader_;
	std::map<std::string, Shader> shaders_;
};