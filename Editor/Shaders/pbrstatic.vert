#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(location = 0) out VS_OUT {
    vec3 WorldPos;
    vec2 TexCoords;
    mat3 TBN;
} vs_out;

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
    vec3 T = normalize(vec3(transform.Transform * vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(transform.Transform * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(transform.Transform * vec4(aNormal,    0.0)));
    vs_out.WorldPos = worldPos.xyz;
    vs_out.TexCoords = aTexCoords;
    vs_out.TBN = mat3(T, B, N);
    gl_Position = uCamera.proj * uCamera.view * worldPos;
}