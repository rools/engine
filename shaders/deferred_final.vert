#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

layout(location = 0) in vec2 position;

out vec2 vertex_position;

void main() {
	gl_Position = vec4(position * 2.0 - 1.0, 0.0, 1.0);
	vertex_position = position;
}