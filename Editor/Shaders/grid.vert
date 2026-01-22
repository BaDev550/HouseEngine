#version 450

layout(set = 0, binding = 0) uniform CameraUniformData {
	mat4 view;
	mat4 proj;
    vec3 position;
} uCamera;

float GridSize = 100.0f;
const vec3 Pos[4] = vec3[](
    vec3(-1.0, 0.0, -1.0),
    vec3( 1.0, 0.0, -1.0),
    vec3( 1.0, 0.0,  1.0),
    vec3(-1.0, 0.0,  1.0)
);

const int Indices[6] = int[6](
    0, 2, 1,
    2, 0, 3
);

layout(location = 0) out float vGridSize;
layout(location = 1) out vec3 vWorldPos;
layout(location = 2) out vec3 vCameraPosition;

void main()
{
    int Index = Indices[gl_VertexIndex];
    vec3 vPos3D = Pos[Index] * GridSize;

    vPos3D.x += uCamera.position.x;
    vPos3D.z += uCamera.position.z;
    vGridSize = GridSize;
    vCameraPosition = uCamera.position;

    vec4 vPos = vec4(vPos3D, 1.0);
    gl_Position = uCamera.proj * uCamera.view * vPos;

    vWorldPos = vPos3D;
}
