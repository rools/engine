#version 150 core

in vec3 vertex_position;
in vec4 vertex_color;
in vec3 vertex_normal;

out vec4 diffuseOutput;
out vec4 positionOutput;
out vec4 normalOutput;

void main() {
	diffuseOutput = vertex_color;
	positionOutput = vec4(vertex_position, 1.0);
	normalOutput = vec4(vertex_normal, 1.0);
}