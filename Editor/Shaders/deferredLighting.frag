#version 450
#extension GL_GOOGLE_include_directive : enable

layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec2 vTexCoords;

#include "Common/buffers.glslh"
#include "Common/PBR.glslh"

layout(set = 1, binding = 0) uniform sampler2D uPosition;
layout(set = 1, binding = 1) uniform sampler2D uNormal;
layout(set = 1, binding = 2) uniform sampler2D uAlbedo;

const float AMBIENT = 0.5f;

void main() {
    vec3 fragPos = texture(uPosition, vTexCoords).rgb;
    vec3 normal  = texture(uNormal, vTexCoords).rgb;
    vec4 albedo  = texture(uAlbedo, vTexCoords);

    float roughness = 0.5f;
    float metallic = 0.5f; // TODO - Move this into material buffer

    vec3 f0 = mix(vec3(0.04), albedo.rgb, metallic);
    vec3 Vdir = normalize(uCamera.position - fragPos);

    vec3 directLighting = vec3(0.0);
    {
        vec3 L = normalize(-uDirectionalLight.Light.Direction);
        vec3 radiance = uDirectionalLight.Light.Color * uDirectionalLight.Light.Intensity;
        directLighting += BRDF(L, Vdir, normal, f0, roughness, albedo.rgb, metallic) * radiance;
    }

    for (int i = 0; i < uPointLights.Count; i++){
        PointLight light = uPointLights.Lights[i];
        vec3 L = normalize(light.Position - fragPos);
        float distance = length(light.Position - fragPos);

        float attenuation = clamp(1.0 - (distance / light.Radius), 0.0, 1.0);
        attenuation *= attenuation;

        vec3 radiance = light.Color * light.Intensity * attenuation;
        directLighting += BRDF(L, Vdir, normal, f0, roughness, albedo.rgb, metallic) * radiance;
    }
    vec3 ambient = vec3(AMBIENT) * albedo.rgb;
    vec3 finalColor = ambient + directLighting;

    finalColor = finalColor / (finalColor + vec3(1.0));
    finalColor = pow(finalColor, vec3(1.0/2.2));
    FragColor = vec4(finalColor, 1.0);
}