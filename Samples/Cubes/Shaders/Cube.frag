#version 460

struct Light {
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout(std140, set = 0, binding = 0) uniform PassBuffer {
    mat4 view;
    mat4 proj;
    Light light;
    vec3 viewPos;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    uint texIndex;
};

layout(std140, set = 1, binding = 0) readonly buffer MaterialBuffer {
    Material materials[];
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

layout(location = 0) in vec3 inPosW;
layout(location = 1) in vec3 inNormalW;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    uint matIndex = renderObject.materialIndex;
    vec3 ambient = light.ambient * materials[matIndex].ambient;

    vec3 normal = normalize(inNormalW);
    vec3 lightDir = normalize(light.pos - inPosW);
    vec3 diffuse = max(dot(normal, lightDir), 0.f) * light.diffuse * materials[matIndex].diffuse;

    vec3 viewDir = normalize(viewPos - inPosW);
    vec3 reflectDir = reflect(-lightDir, inNormalW);
    vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), materials[matIndex].shininess) * light.specular * materials[matIndex].specular;

    outColor = vec4(ambient + diffuse + specular, 1.f);
}
