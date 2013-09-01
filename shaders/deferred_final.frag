#version 150 core

uniform sampler2D diffuseTexture;
uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D lightTexture;

in vec2 vertex_position;

out vec4 fragColor;

void main() {
	// Hard code a directional light until directional lights are implemented
	const vec3 ambientLightDir = vec3(0.208878, -0.919063, 0.334205);

	vec4 diffuse = texture(diffuseTexture, vertex_position);
	vec4 pos = texture(positionTexture, vertex_position);
	vec4 normal = texture(normalTexture, vertex_position);
	vec4 light = texture(lightTexture, vertex_position);

	// Ambient light
	fragColor = 0.2 * diffuse;

	// Directional light
	fragColor += 0.2 * diffuse * max(dot(normal.xyz, ambientLightDir), 0.0);

	// Other light sources
	fragColor += 0.8 * light * diffuse;

	//fragColor = vec4(vertex_position.x, vertex_position.y, 0.0, 1.0);
}
