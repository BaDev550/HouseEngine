#version 450

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTextureCoords;

layout(set = 1, binding = 0) uniform sampler2D diffTexture;

void main(){
	vec4 texColor = texture(diffTexture, fragTextureCoords);
	FragColor = texColor;
}