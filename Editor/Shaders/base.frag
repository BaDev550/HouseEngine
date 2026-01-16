#version 450

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec2 fragTextureCoords;

//layout(set = 1, binding = 0) uniform sampler2D diffTexture;
//layout(set = 1, binding = 1) uniform sampler2D normTexture;

void main(){
	FragColor = vec4(1.0, 1.0, 1.0, 1.0f);
}