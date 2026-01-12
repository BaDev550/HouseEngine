#version 450

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;
layout(location = 1) in vec3 aNormal;

layout(location = 0) out vec2 fragTextureCoords;

layout(set = 0, binding = 0) uniform CameraUniformData {
	mat4 view;
	mat4 proj;
} camera;

void main() {
	gl_Position = camera.proj * camera.view * vec4(aPos, 1.0);
	fragTextureCoords = aTexCoords;
}