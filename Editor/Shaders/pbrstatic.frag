#version 450
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outAlbedo;

layout(location = 0) in VS_OUT {
    vec3 WorldPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

#include "Common/PBRResources.glslh"

void main(){
    outPosition = vec4(fs_in.WorldPos, 1.0);

    vec3 normal = texture(uNormTexture, fs_in.TexCoords).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(fs_in.TBN * normal);
    
    float roughness = uMaterial.roughness;
    outNormal = vec4(normal, roughness);

    vec3 albedoTextureColor = texture(uDiffTexture, fs_in.TexCoords).rgb;
    vec3 albedo = albedoTextureColor;
    outAlbedo = vec4(albedoTextureColor, 1.0f);
}