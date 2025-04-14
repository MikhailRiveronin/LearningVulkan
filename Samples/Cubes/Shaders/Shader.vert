#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoords;

layout(location = 0) out vec2 outTexCoords;

layout(binding = 0) uniform UniformBufferObjects {
    mat4 world;
    mat4 view;
    mat4 proj;
} ubo;

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.world * vec4(inPosition, 1.f);
    outTexCoords = inTexCoords;
}
