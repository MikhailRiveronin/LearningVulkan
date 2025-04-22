#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTexCoords;

layout(binding = 0) uniform UniformBufferObjects {
    mat4 world;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) out vec2 outTexCoord;
void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.world * vec4(inPos, 1.f);
    outTexCoord = inTexCoords;
}
