#version 450
layout(location = 0) out float FragColor;
layout(location = 0) in vec2 vTexCoords;

layout(set = 0, binding = 0) uniform sampler2D uPosition;
layout(set = 0, binding = 1) uniform sampler2D uNormal;
layout(set = 0, binding = 2) uniform sampler2D uTexNoise;
layout(set = 0, binding = 3) uniform SSAOKernel {
    vec3 samples[64];
} uKernel;

layout(set = 0, binding = 4) uniform CameraUniformData {
	mat4 view;
	mat4 proj;
    vec3 position;
} uCamera;

const int kernelSize = 64;
const float radius = 0.5;
const float bias = 0.025;

void main() {
    vec3 worldPos = texture(uPosition, vTexCoords).rgb;
    vec3 fragPos  = vec3(uCamera.view * vec4(worldPos, 1.0));

    vec3 worldNormal = normalize(texture(uNormal, vTexCoords).rgb);
    vec3 normal      = normalize(mat3(uCamera.view) * worldNormal);
    if (length(worldNormal) < 0.1) {
        FragColor = 1.0; 
        return;
    }

    ivec2 screenSize = textureSize(uPosition, 0);
    vec2 noiseScale  = vec2(screenSize.x / 4.0, screenSize.y / 4.0);
    vec3 randomVec   = normalize(texture(uTexNoise, vTexCoords * noiseScale).xyz);

    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i) {
        vec3 samplePos = TBN * uKernel.samples[i];
        samplePos = fragPos + samplePos * radius; 
        
        vec4 offset = vec4(samplePos, 1.0);
        offset = uCamera.proj * offset; 
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5; 
        
        vec3 geometryWorldPos = texture(uPosition, offset.xy).rgb;
        float sampleDepth = (uCamera.view * vec4(geometryWorldPos, 1.0)).z;
        
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    
    occlusion = 1.0 - (occlusion / kernelSize);
    FragColor = occlusion;
}