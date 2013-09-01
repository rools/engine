#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 cameraPosition;

out vec3 vertex_position;
out vec4 vertex_color;
out vec3 vertex_normal;

void main() {
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);

	vertex_position = (modelMatrix * vec4(position, 1.0)).xyz;
	vertex_color = color;
	vertex_normal = mat3(modelMatrix) * normal;

	// Flip normal if the triangle's back is facing the camera
	vec3 triangleDirection = normalize(vertex_position - cameraPosition);
	vertex_normal = faceforward(-vertex_normal, triangleDirection, vertex_normal);
}
