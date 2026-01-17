#version 450

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outAlbedo;

layout (location = 0) in vec3 vWorldPos;
layout (location = 1) in vec2 vTexCoords;
layout (location = 2) in vec3 vNormal;

layout(set = 1, binding = 0) uniform sampler2D diffTexture;
layout(set = 1, binding = 1) uniform sampler2D normTexture;

void main(){
	outPosition = vec4(vWorldPos, 1.0);
    outNormal   = vec4(normalize(vNormal), 1.0);
    outAlbedo   = texture(diffTexture, vTexCoords);
}