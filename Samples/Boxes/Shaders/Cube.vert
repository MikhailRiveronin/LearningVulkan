#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout(std140, set = 0, binding = 0) uniform PassBuffer {
    mat4 view;
    mat4 proj;
    vec3 viewPos;
};

struct RenderObject {
    mat4 world;
    mat4 texTransform;
    uint meshIndex;
    uint submeshIndex;
    uint materialIndex;
};

layout(std140, set = 2, binding = 0) uniform RenderObjectBuffer {
    RenderObject renderObject;
};

layout(location = 0) out vec3 outPosW;
layout(location = 1) out vec3 outNormalW;
layout(location = 2) out vec2 outTexCoord;

void main()
{
    outPosW = vec3(renderObject.world * vec4(inPos, 1.f));
    outNormalW = mat3(transpose(inverse(renderObject.world))) * inNormal;
    outTexCoord = inTexCoord;
    gl_Position = proj * view * renderObject.world * vec4(inPos, 1.f);
}
