#include "engine/utils/ShaderManager.h"

#include "engine/components/Transform.h"
#include "engine/Entity.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

#define GLDBG printf("%s:%i - %x\n", __FILE__, __LINE__, glGetError());

ShaderManager &ShaderManager::getInstance() {
	static ShaderManager instance;
	return instance;
}

ShaderManager::~ShaderManager() {
	// Delete all programs
	glUseProgram(0);
	for (const auto &shader : shaders_)
		glDeleteProgram(shader.second.program);
}

void ShaderManager::loadShader(const std::string &name) {
	// Check if shader is already compiled
	if (shaders_.find(name) != shaders_.end()) {
		const Shader &shader = shaders_[name];
		loadedShader_ = shader;
		glUseProgram(shader.program);
		return;
	}

	Shader shader;

	// Load and compile vertex and fragment shader
	shader.vertexShader = compileShader(name + ".vert", GL_VERTEX_SHADER, false);
	shader.geometryShader = compileShader(name + ".geom", GL_GEOMETRY_SHADER, true);
	shader.fragmentShader = compileShader(name + ".frag", GL_FRAGMENT_SHADER, false);

	// Create program and attach shaders
	shader.program = glCreateProgram();
	glAttachShader(shader.program, shader.vertexShader);
	if (shader.geometryShader != 0)
		glAttachShader(shader.program, shader.geometryShader);
	glAttachShader(shader.program, shader.fragmentShader);

	//glBindAttribLocation(shader.program, 0, "position");
	//glBindAttribLocation(shader.program, 1, "normal");

	// For deferred shader
	glBindFragDataLocation(shader.program, 0, "diffuseOutput");
	glBindFragDataLocation(shader.program, 1, "positionOutput");
	glBindFragDataLocation(shader.program, 2, "normalOutput");
	glBindFragDataLocation(shader.program, 3, "lightOutput");

	glLinkProgram(shader.program);

	glUseProgram(shader.program);

	// Get locations of uniforms
	shader.modelMatrix = glGetUniformLocation(shader.program, "modelMatrix");
	shader.viewMatrix = glGetUniformLocation(shader.program, "viewMatrix");
	shader.projectionMatrix = glGetUniformLocation(shader.program, "projectionMatrix");

	// Get locations of optional uniforms
	shader.resolution = glGetUniformLocation(shader.program, "resolution");
	shader.cameraPosition = glGetUniformLocation(shader.program, "cameraPosition");
	shader.lightPosition = glGetUniformLocation(shader.program, "lightPosition");
	shader.lightColor = glGetUniformLocation(shader.program, "lightColor");
	shader.lightRange = glGetUniformLocation(shader.program, "lightRange");

	GLint diffuseTexture = glGetUniformLocation(shader.program, "diffuseTexture");
	GLint positionTexture = glGetUniformLocation(shader.program, "positionTexture");
	GLint normalTexture = glGetUniformLocation(shader.program, "normalTexture");
	GLint lightTexture = glGetUniformLocation(shader.program, "lightTexture");

	if (diffuseTexture != -1)
		glUniform1i(diffuseTexture, 0);
	if (positionTexture != -1)
		glUniform1i(positionTexture, 1);
	if (normalTexture != -1)
		glUniform1i(normalTexture, 2);
	if (lightTexture != -1)
		glUniform1i(lightTexture, 3);

	// Flag shaders for deletion, which will occur when their programs are deleted
	glDeleteShader(shader.vertexShader);
	if (shader.geometryShader != 0)
		glDeleteProgram(shader.geometryShader);
	glDeleteShader(shader.fragmentShader);

	// Save program for fast future loading
	shaders_[name] = shader;

	loadedShader_ = shader;
}

void ShaderManager::setModelMatrix(const glm::mat4 &matrix) {
	glUniformMatrix4fv(loadedShader_.modelMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderManager::setModelMatrix(const btTransform &transform) {
	btScalar matrix[16];
	transform.getOpenGLMatrix(matrix);
	glUniformMatrix4fv(loadedShader_.modelMatrix, 1, GL_FALSE, matrix);
}

void ShaderManager::setViewMatrix(const glm::mat4 &matrix) {
	glUniformMatrix4fv(loadedShader_.viewMatrix, 1, GL_FALSE, glm::value_ptr(matrix));

	if (loadedShader_.cameraPosition != -1) {
		glm::vec4 position = matrix * glm::vec4(0.0, 0.0, 0.0, 0.0);
		glUniform3fv(loadedShader_.cameraPosition, 1, glm::value_ptr(position));
	}
}

void ShaderManager::setViewMatrix(const btTransform &transform) {
	btScalar matrix[16];
	transform.getOpenGLMatrix(matrix);
	glUniformMatrix4fv(loadedShader_.viewMatrix, 1, GL_FALSE, matrix);

	if (loadedShader_.cameraPosition != -1) {
		btVector3 position = transform.inverse().getOrigin();
		glUniform3f(loadedShader_.cameraPosition, position.x(), position.y(), position.z());
	}
}

void ShaderManager::setProjectionMatrix(const glm::mat4 &matrix) {
	glUniformMatrix4fv(loadedShader_.projectionMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderManager::setResolution(int width, int height) {
	glUniform2f(loadedShader_.resolution, width, height);
}

void ShaderManager::setCamera(const Entity *entity) {

}

void ShaderManager::setLight(const Entity *entity) {
	const btVector3 &position = entity->get<Transform>()->transform().getOrigin();
	glUniform3f(loadedShader_.lightPosition, position.x(), position.y(), position.z());

	const Color color = entity->get<Light>()->color();
	glUniform3f(loadedShader_.lightColor, color.red / 255.0, color.green / 255.0, color.blue / 255.0);

	const float range = entity->get<Light>()->range();;
	glUniform1f(loadedShader_.lightRange, range);
}

void ShaderManager::validateCurrentShader() {
	GLint isValid;
	glValidateProgram(loadedShader_.program);
	glGetProgramiv(loadedShader_.program, GL_VALIDATE_STATUS, &isValid);
	if (isValid != GL_TRUE) {
		char logBuffer[1000];
		GLsizei logLength;
		glGetProgramInfoLog(loadedShader_.program, sizeof(logBuffer), &logLength, logBuffer);
		throw std::runtime_error("Shader program not valid: " + std::string(logBuffer));
	}
}

GLuint ShaderManager::compileShader(const std::string &fileName, GLenum type, bool optional) {
	std::ifstream file(fileName);

	// Check whether the file actually exists
	if (!file) {
		if (optional)
			return 0;
		else
			throw std::runtime_error("Could not open shader " + fileName);
	}

	// Load shader source
	std::stringstream sourceStream;
	sourceStream << file.rdbuf();
	file.close();
	const std::string &sourceString = sourceStream.str();
	const char *source = sourceString.c_str();
	GLint sourceLength = sourceString.size();

	// Create and compile shader
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, &sourceLength);
	glCompileShader(shader);

	// If an error occured during shader compilation, throw exception with log
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		char logBuffer[1000];
		GLsizei logLength;
		glGetShaderInfoLog(shader, sizeof(logBuffer), &logLength, logBuffer);
		glDeleteShader(shader);
		throw std::runtime_error("Error compiling shader " + fileName + ":\n" + logBuffer);
	}

	return shader;
}