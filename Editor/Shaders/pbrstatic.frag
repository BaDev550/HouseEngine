#version 450
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outAlbedo;

layout(location = 0) in vec3 vWorldPos;
layout(location = 1) in vec2 vTexCoords;
layout(location = 2) in vec3 vNormal;

#include "Common/PBRResources.glslh"

void main(){
	outPosition = vec4(vWorldPos, 1.0);
    outNormal   = vec4(normalize(vNormal), 1.0);
    outAlbedo   = texture(uDiffTexture, vTexCoords);
}