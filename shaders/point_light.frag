#version 150 core

uniform vec2 resolution;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightRange;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;

in vec4 vertex_color;

out vec4 lightOutput;

void main() {
	vec2 textureCoordinate = gl_FragCoord.xy / resolution;
	vec3 position = texture(positionTexture, textureCoordinate).xyz;
	vec3 normal = texture(normalTexture, textureCoordinate).xyz;

	vec3 lightDirection = normalize(position - lightPosition);

	//float amount = max(1.0 - 0.2 * length(position - lightPosition), 0.0) * max(dot(lightDirection, normal), 0.0);

	// Quadratic attenuation
	//float attenuation = 1.0 / (1.0 + 1.0 * distance(position, lightPosition) * distance(position, lightPosition)) - 1.0 / (lightRange * lightRange + 1);

	// Linear attenuation
	float attenuation = max(1.0 - 0.2 * distance(position, lightPosition), 0.0);

	attenuation *= max(dot(lightDirection, normal), 0.0);

	lightOutput = vec4(attenuation * lightColor, 1.0);
}