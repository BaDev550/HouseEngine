#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;

layout(location = 0) out vec2 fragTextureCoords;
layout(location = 1) out float depth;

layout(push_constant) uniform TransformUniformData {
	mat4 model;
} transform;

layout(set = 0, binding = 0) uniform CameraUniformData {
	mat4 view;
	mat4 proj;
} camera;

void main() {
	fragTextureCoords = aTexCoords;
	vec4 worldPos = camera.proj * camera.view * transform.model * vec4(aPos, 1.0);
	depth = worldPos.z;
	gl_Position = worldPos;
}