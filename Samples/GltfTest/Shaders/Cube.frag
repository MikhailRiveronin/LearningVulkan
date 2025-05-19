#version 460
#extension GL_EXT_nonuniform_qualifier : enable

layout(std140, set = 0, binding = 0) uniform PassBuffer {
    mat4 view;
    mat4 proj;
    vec3 viewPos;
};

struct Material {
    float shininess;
    uint diffTexIndex;
};

layout(std140, set = 1, binding = 0) readonly buffer MaterialBuffer {
    Material materials[];
};

layout(set = 1, binding = 1) uniform sampler2D Textures[];

layout(push_constant) uniform PushConstants
{
    uint materialIndex;
};

layout(location = 0) in vec3 inPosW;
layout(location = 1) in vec3 inNormalW;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    float z = gl_FragCoord.z;
    outColor = texture(Textures[materialIndex], inTexCoord);
}
