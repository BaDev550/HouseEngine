#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;

layout(location = 0) out vec3 vWorldPos;
layout(location = 1) out vec2 vTexCoords;
layout(location = 2) out vec3 vNormal;

layout(push_constant) uniform TransformUniformData {
	mat4 Transform;
} transform;

layout(set = 0, binding = 0) uniform CameraUniformData {
	mat4 view;
	mat4 proj;
    vec3 position;
} uCamera;

void main() {
	vec4 worldPos = transform.Transform * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    vTexCoords = aTexCoords;
    vNormal = mat3(transpose(inverse(transform.Transform))) * aNormal;
    gl_Position = uCamera.proj * uCamera.view * worldPos;
}