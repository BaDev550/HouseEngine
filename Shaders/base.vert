#version 450

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoords;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTextureCoords;

layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

void main() {
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(aPos, 0.0, 1.0);
	fragColor = aColor;
	fragTextureCoords = aTexCoords;
}