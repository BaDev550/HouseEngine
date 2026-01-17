#version 450

layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec2 vTexCoords;

layout(set = 0, binding = 0) uniform Camera 
{ 
    mat4 view; 
    mat4 proj; 
} camera;

layout(set = 1, binding = 0) uniform sampler2D uPosition;
layout(set = 1, binding = 1) uniform sampler2D uNormal;
layout(set = 1, binding = 2) uniform sampler2D uAlbedo;

void main() {
    vec3 fragPos = texture(uPosition, vTexCoords).rgb;
    vec3 normal  = texture(uNormal, vTexCoords).rgb;
    vec4 albedo  = texture(uAlbedo, vTexCoords);

    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(normal, lightDir), 0.0);
    
    FragColor = vec4(albedo.rgb * diff, 1.0);
}