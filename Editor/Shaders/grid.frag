#version 450

layout(location = 0) out vec4 FragColor;
layout(location = 0) in float vGridSize;
layout(location = 1) in vec3  vWorldPos;
layout(location = 2) in vec3  vCameraPosition;

float GridCellSize = 0.15f;
float GridMinPixelsBetweenCells = 2.0;
vec4 GridColorThin = vec4(0.0f, 0.0f, 0.0f, 1.0f);
vec4 GridColorThick = vec4(0.5f, 0.5f, 0.5f, 1.0f);

float log10(float x)
{
    float f = log(x) / log(10.0);
    return f;
}


float satf(float x)
{
    float f = clamp(x, 0.0, 1.0);
    return f;
}


vec2 satv(vec2 x)
{
    vec2 v = clamp(x, vec2(0.0), vec2(1.0));
    return v;
}


float max2(vec2 v)
{
    float f = max(v.x, v.y);
    return f;
}

void main()
{
    vec2 dvx = vec2(dFdx(vWorldPos.x), dFdy(vWorldPos.x));
    vec2 dvy = vec2(dFdx(vWorldPos.z), dFdy(vWorldPos.z));

    float lx = length(dvx);
    float ly = length(dvy);

    vec2 dudv = vec2(lx, ly);

    float l = length(dudv);

    float LOD = max(0.0, log10(l * GridMinPixelsBetweenCells / GridCellSize) + 1.0);

    float GridCellSizeLod0 = GridCellSize * pow(10.0, floor(LOD));
    float GridCellSizeLod1 = GridCellSizeLod0 * 10.0;
    float GridCellSizeLod2 = GridCellSizeLod1 * 10.0;

    dudv *= 4.0;

    vec2 mod_div_dudv = mod(vWorldPos.xz, GridCellSizeLod0) / dudv;
    float Lod0a = max2(vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0)) );

    mod_div_dudv = mod(vWorldPos.xz, GridCellSizeLod1) / dudv;
    float Lod1a = max2(vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0)) );
    
    mod_div_dudv = mod(vWorldPos.xz, GridCellSizeLod2) / dudv;
    float Lod2a = max2(vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0)) );

    float LOD_fade = fract(LOD);
    vec4 Color;

    if (Lod2a > 0.0) {
        Color = GridColorThick;
        Color.a *= Lod2a;
    } else {
        if (Lod1a > 0.0) {
            Color = mix(GridColorThick, GridColorThin, LOD_fade);
	        Color.a *= Lod1a;
        } else {
            Color = GridColorThin;
	        Color.a *= (Lod0a * (1.0 - LOD_fade));
        }
    }
    
    float OpacityFalloff = (1.0 - satf(length(vWorldPos.xz - vCameraPosition.xz) / vGridSize));

    Color.a *= OpacityFalloff;

    FragColor = Color;
}
