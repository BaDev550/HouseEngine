#version 450
#extension GL_GOOGLE_include_directive : enable

layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec2 vTexCoords;

#include "Common/buffers.glslh"

layout(set = 1, binding = 0) uniform sampler2D uPosition;
layout(set = 1, binding = 1) uniform sampler2D uNormal;
layout(set = 1, binding = 2) uniform sampler2D uAlbedo;

const float AMBIENT = 0.5f;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos){
    vec3 lightDir = normalize(light.Position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    float distance = length(light.Position - fragPos);
    float attenuation = clamp(1.0 - (distance / light.Radius), 0.0, 1.0);
    attenuation *= attenuation;
    vec3 diffuse = diff * light.Color * light.Intensity;
    return diffuse * attenuation;
}

void main() {
    vec3 fragPos = texture(uPosition, vTexCoords).rgb;
    vec3 normal  = texture(uNormal, vTexCoords).rgb;
    vec4 albedo  = texture(uAlbedo, vTexCoords);
    float diff = max(dot(normal, uDirectionalLight.Light.Direction), 0.0);
    
    vec3 dirLightDir = normalize(-uDirectionalLight.Light.Direction);
    float dirDiff = max(dot(normal, dirLightDir), 0.0);
    vec3 lighting = dirDiff * uDirectionalLight.Light.Color * uDirectionalLight.Light.Intensity + AMBIENT;

    for (int i = 0; i < uPointLights.Count; i++)
        lighting += CalcPointLight(uPointLights.Lights[i], normal, fragPos);
    vec3 finalColor = albedo.rgb * lighting;

    FragColor = vec4(finalColor, 1.0);
}