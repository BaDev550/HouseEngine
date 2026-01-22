#version 450
#extension GL_GOOGLE_include_directive : enable

layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec2 vTexCoords;

#include "Common/buffers.glslh"
#include "Common/PBR.glslh"

layout(set = 1, binding = 0) uniform sampler2D uPosition;
layout(set = 1, binding = 1) uniform sampler2D uNormal;
layout(set = 1, binding = 2) uniform sampler2D uAlbedo;

const float AMBIENT = 0.02f;

void main() {
    vec3 fragPos = texture(uPosition, vTexCoords).rgb;
    vec4 normal  = texture(uNormal, vTexCoords);
    vec4 albedo  = texture(uAlbedo, vTexCoords);
    vec3  A = albedo.rgb;
    vec3  N = normal.rgb;
    float R = normal.a;
    float M = albedo.a;

    vec3 f0 = mix(vec3(0.04), A, M);
    vec3 Vdir = normalize(uCamera.position - fragPos);

    vec3 directLighting = vec3(0.0);
    {
        vec3 L = normalize(-uDirectionalLight.Light.Direction);
        vec3 radiance = uDirectionalLight.Light.Color * uDirectionalLight.Light.Intensity;
        directLighting += BRDF(L, Vdir, N, f0, R, albedo.rgb, M) * radiance;
    }

    for (int i = 0; i < uPointLights.Count; i++){
        PointLight light = uPointLights.Lights[i];
        vec3 L = normalize(light.Position - fragPos);
        float distance = length(light.Position - fragPos);

        float attenuation = clamp(1.0 - (distance / light.Radius), 0.0, 1.0);
        attenuation *= attenuation;

        vec3 radiance = light.Color * light.Intensity * attenuation;
        directLighting += BRDF(L, Vdir, N, f0, R, albedo.rgb, M) * radiance;
    }
    vec3 ambient = vec3(AMBIENT) * albedo.rgb;
    vec3 finalColor = ambient + directLighting;

    finalColor = finalColor / (finalColor + vec3(1.0));
    finalColor = pow(finalColor, vec3(1.0/2.2));
    FragColor = vec4(finalColor, 1.0);
}