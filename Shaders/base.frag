#version 450

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec2 fragTextureCoords;

//layout(set = 1, binding = 0) uniform sampler2D diffTexture;

void main(){
	//vec4 texColor = texture(diffTexture, fragTextureCoords);
	vec4 texColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	FragColor = texColor;
}