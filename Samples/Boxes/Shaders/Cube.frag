#version 460
#extension GL_EXT_nonuniform_qualifier : enable

layout(std140, set = 0, binding = 0) uniform PassBuffer {
    mat4 view;
    mat4 proj;
    vec3 viewPos;
};

struct DirLight {
    vec3 dir;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout(std140, set = 0, binding = 1) uniform DirLightBuffer {
    DirLight dirLight;
};

struct SpotLight {
    vec3 pos;
    vec3 dir;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;
    float outerCutOff;
    float constAtt;
    float linearAtt;
    float quadAtt;
};

layout(std140, set = 0, binding = 2) uniform SpotLightBuffer {
    SpotLight spotLight;
};

struct PointLight {
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constAtt;
    float linearAtt;
    float quadAtt;
    uint padding[2];
};

layout(std140, set = 0, binding = 3) readonly buffer PointLightBuffer {
    PointLight pointLights[];
};

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffTexel, vec3 specTexel, float shininess);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffTexel, vec3 specTexel, float shininess);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffTexel, vec3 specTexel, float shininess);

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    uint diffTexIndex;
    uint specTexIndex;
};

layout(std140, set = 1, binding = 0) readonly buffer MaterialBuffer {
    Material materials[];
};

layout(set = 1, binding = 1) uniform sampler2D Textures[];

struct RenderObject {
    mat4 world;
    mat4 texTransform;
    uint meshIndex;
    uint submeshIndex;
    uint matIndex;
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
    vec3 normal = normalize(inNormalW);
    vec3 viewDir = normalize(viewPos - inPosW);
    uint matIndex = renderObject.matIndex;
    float shininess = materials[matIndex].shininess;
    vec3 diffTexel = texture(Textures[materials[matIndex].diffTexIndex], inTexCoord).rgb;
    vec3 specTexel = texture(Textures[materials[matIndex].specTexIndex], inTexCoord).rgb;

    vec3 result = calcDirLight(dirLight, normal, viewDir, diffTexel, specTexel, shininess);
    result += calcSpotLight(spotLight, normal, inPosW, viewDir, diffTexel, specTexel, shininess);

    for (uint i = 0; i < 4; ++i) {
        result += calcPointLight(pointLights[i], normal, inPosW, viewDir, diffTexel, specTexel, shininess);
    }

    outColor = vec4(result, 1.f);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffTexel, vec3 specTexel, float shininess)
{
    vec3 lightDir = normalize(-light.dir);
    vec3 reflectReverseLightDir = reflect(light.dir, normal);

    vec3 ambient = light.ambient * diffTexel;

    float diff = max(dot(lightDir, normal), 0.f);
    vec3 diffuse = diff * light.diffuse * diffTexel;

    float spec = pow(max(dot(reflectReverseLightDir, viewDir), 0.f), shininess);
    vec3 specular = spec * light.specular * specTexel;

    return ambient + diffuse + specular;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffTexel, vec3 specTexel, float shininess)
{
    vec3 lightDir = normalize(light.pos - fragPos);
    vec3 reflectReverseLightDir = reflect(-light.dir, normal);

    float theta = dot(normalize(-light.dir), lightDir);
    float intensity = clamp((theta - light.outerCutOff) / (light.cutOff - light.outerCutOff), 0.f, 1.f);

    float dist = length(light.pos - fragPos);
    float att = 1.f / (light.constAtt + light.linearAtt * dist + light.quadAtt * dist * dist);

    vec3 ambient = att * light.ambient * diffTexel;

    float diff = max(dot(lightDir, normal), 0.f);
    vec3 diffuse = att * intensity * diff * light.diffuse * diffTexel;

    float spec = pow(max(dot(reflectReverseLightDir, viewDir), 0.f), shininess);
    vec3 specular = att * intensity * spec * light.specular * specTexel;

    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffTexel, vec3 specTexel, float shininess)
{
    vec3 lightDir = normalize(light.pos - fragPos);
    vec3 reflectReverseLightDir = reflect(-lightDir, normal);

    float dist = length(light.pos - fragPos);
    float att = 1.f / (light.constAtt + light.linearAtt * dist + light.quadAtt * dist * dist);

    vec3 ambient = att * light.ambient * diffTexel;

    float diff = max(dot(lightDir, normal), 0.f);
    vec3 diffuse = att * diff * light.diffuse * diffTexel;

    float spec = pow(max(dot(reflectReverseLightDir, viewDir), 0.f), shininess);
    vec3 specular = att * spec * light.specular * specTexel;

    return ambient + diffuse + specular;
}
