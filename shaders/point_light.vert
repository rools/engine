#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 vertex_color;

void main() {
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);

	vertex_color = color;
}
