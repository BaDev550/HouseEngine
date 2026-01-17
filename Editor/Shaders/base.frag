#version 450

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec2 fragTextureCoords;
layout(location = 1) in float depth;

layout(set = 1, binding = 0) uniform sampler2D diffTexture;
layout(set = 1, binding = 1) uniform sampler2D normTexture;

void main(){
	vec4 diffuseColor = texture(diffTexture, fragTextureCoords);
	//FragColor = vec4(vec3(1.0f - depth), 1.0f); debug depth;
	FragColor = diffuseColor;
}