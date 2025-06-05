#include "Boilerplate/Application.h"
#include "Boilerplate/Entry.h"
#include "Boilerplate/EventManager.h"
#include "Boilerplate/Initializer.h"
#include "Boilerplate/ProceduralMeshes/Box.h"
#include "Boilerplate/ProceduralMeshes/Sphere.h"
#include "Boilerplate/SampleBase.h"
#include "Boilerplate/Structures.h"
#include "Boilerplate/Utils.h"

#include <stb_image.h>
#include <glm/gtc/matrix_transform.hpp>

class Boxes : public SampleBase {
public:
    Boxes(u32 width, u32 height, std::string const& name);

private:
    std::vector<Mesh> meshes;
    std::vector<Texture> textures;
    std::vector<Material> materials;
    std::vector<RenderObject> renderObjects;
    std::vector<FrameResource> frameResources;
    std::vector<DescriptorSets> resourceDescriptors;
    std::vector<VkPushConstantRange> pushConstantRanges;
    std::vector<VkPipelineLayout> pipelineLayouts;
    std::vector<VkPipeline> pipelines;

    struct {
        DirLight direct;
        SpotLight spot;
        std::vector<PointLight> points;
    } lights;
    std::vector<glm::vec3> pointLightPositions;

    Camera camera;
    glm::vec3 lightPos = glm::vec3(1.2f, 1.f, 2.f);
    glm::vec3 lightColor = glm::vec3(1.f);
    glm::vec3 diffuseColor;
    glm::vec3 ambientColor;

    void createMeshes() override;
    void createTextures() override;
    void createMaterials() override;
    void createRenderObjects() override;
    void createLights() override;
    void createFrameResources() override;
    void createResourceDescriptors() override;
    void createPushConstantRanges() override;
    void createPipelines() override;

    void updateFrameResources(u32 frameIndex) override;
    void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex, ImDrawData* draw_data) override;

    void destroyPipelines() override;
    void destroyPushConstantRanges() override;
    void destroyResourceDescriptors() override;
    void destroyFrameResources() override;
    void destroyTextures() override;
    void destroyMeshes() override;
};

Boxes::Boxes(u32 width, u32 height, std::string const& name) :
    SampleBase(width, height, name),
    camera(globals, glm::vec3(0.f, 0.f, 5.f))
{
    EventManager::subscribe(EventType::LEFT_BUTTON_DOWN, &camera);
    EventManager::subscribe(EventType::MOUSE_MOVE, &camera);
    EventManager::subscribe(EventType::KEY_DOWN, &camera);
    EventManager::subscribe(EventType::WINDOW_RESIZE, this);

    diffuseColor = lightColor * glm::vec3(0.5f);
    ambientColor = diffuseColor * glm::vec3(0.2f);

    pointLightPositions.resize(4);
    pointLightPositions[0] = glm::vec3(0.7f, 0.2f, 2.f);
    pointLightPositions[1] = glm::vec3(2.3f, -3.3f, -4.f);
    pointLightPositions[2] = glm::vec3(-4.f,  2.f, -12.f);
    pointLightPositions[3] = glm::vec3(0.f,  0.f, -3.f);
}

void Boxes::createMeshes()
{
    meshes.resize(1);
    {
        std::vector<Vertex> vertices;
        std::vector<u16> indices;
        meshes[0].submeshes.resize(2);
        {
            meshes[0].submeshes[0].indexCount = Box::indices.size();
            meshes[0].submeshes[0].firstIndex = indices.size();
            meshes[0].submeshes[0].vertexOffset = vertices.size();
            vertices.insert(vertices.end(), Box::vertices.begin(), Box::vertices.end());
            indices.insert(indices.end(), Box::indices.begin(), Box::indices.end());
        }
        {
            meshes[0].submeshes[1].indexCount = Sphere::indices.size();
            meshes[0].submeshes[1].firstIndex = indices.size();
            meshes[0].submeshes[1].vertexOffset = vertices.size();
            vertices.insert(vertices.end(), Sphere::vertices.begin(), Sphere::vertices.end());
            indices.insert(indices.end(), Sphere::indices.begin(), Sphere::indices.end());
        }
        {
            Buffer stagingBuffer;
            stagingBuffer.size = vertices.size() * sizeof(vertices[0]);
            stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            stagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            createBuffer(globals, stagingBuffer);
            THROW_IF_FAILED(
                vkMapMemory(globals.device.handle, stagingBuffer.memory, 0, stagingBuffer.size, 0, &stagingBuffer.mapped),
                __FILE__, __LINE__,
                "Failed to map memory");
            memcpy(stagingBuffer.mapped, vertices.data(), stagingBuffer.size);
            vkUnmapMemory(globals.device.handle, stagingBuffer.memory);

            meshes[0].vertexBuffer.size = stagingBuffer.size;
            meshes[0].vertexBuffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            meshes[0].vertexBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            createBuffer(globals, meshes[0].vertexBuffer);
            copyBuffer(globals, stagingBuffer, meshes[0].vertexBuffer);
            destroyBuffer(globals, stagingBuffer);
        }
        {
            Buffer stagingBuffer;
            stagingBuffer.size = indices.size() * sizeof(indices[0]);
            stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            stagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            createBuffer(globals, stagingBuffer);
            THROW_IF_FAILED(
                vkMapMemory(globals.device.handle, stagingBuffer.memory, 0, stagingBuffer.size, 0, &stagingBuffer.mapped),
                __FILE__, __LINE__,
                "Failed to map memory");
            memcpy(stagingBuffer.mapped, indices.data(), stagingBuffer.size);
            vkUnmapMemory(globals.device.handle, stagingBuffer.memory);

            meshes[0].indexBuffer.size = stagingBuffer.size;
            meshes[0].indexBuffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            meshes[0].indexBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            createBuffer(globals, meshes[0].indexBuffer);
            copyBuffer(globals, stagingBuffer, meshes[0].indexBuffer);
            destroyBuffer(globals, stagingBuffer);
        }
    }
}

void Boxes::createTextures()
{
    textures.resize(2);
    {
        int width;
        int height;
        int channels;
        stbi_uc* pixels = stbi_load("Textures/container2.png", &width, &height, &channels, STBI_rgb_alpha);
        if (!pixels) {
            throw std::runtime_error("Failed to load texture image");
        }
        VkDeviceSize imageSize = width * height * 4;

        Buffer stagingBuffer;
        stagingBuffer.size = imageSize;
        stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        createBuffer(globals, stagingBuffer);
        vkMapMemory(globals.device.handle, stagingBuffer.memory, 0, stagingBuffer.size, 0, &stagingBuffer.mapped);
        memcpy(stagingBuffer.mapped, pixels, imageSize);
        vkUnmapMemory(globals.device.handle, stagingBuffer.memory);
        stbi_image_free(pixels);

        textures[0].image.width = width;
        textures[0].image.height = height;
        textures[0].image.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textures[0].image.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        createImage(globals, textures[0].image);
        transitionImageLayout(globals, textures[0].image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(globals, stagingBuffer, textures[0].image);
        destroyBuffer(globals, stagingBuffer);
        transitionImageLayout(globals, textures[0].image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        createImageView(globals, textures[0].image);
        createSampler(globals, textures[0].sampler);
    }
    {
        int width;
        int height;
        int channels;
        stbi_uc* pixels = stbi_load("Textures/container2_specular.png", &width, &height, &channels, STBI_rgb_alpha);
        if (!pixels) {
            throw std::runtime_error("Failed to load texture image");
        }
        VkDeviceSize imageSize = width * height * 4;

        Buffer stagingBuffer;
        stagingBuffer.size = imageSize;
        stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        createBuffer(globals, stagingBuffer);
        vkMapMemory(globals.device.handle, stagingBuffer.memory, 0, stagingBuffer.size, 0, &stagingBuffer.mapped);
        memcpy(stagingBuffer.mapped, pixels, imageSize);
        vkUnmapMemory(globals.device.handle, stagingBuffer.memory);
        stbi_image_free(pixels);

        textures[1].image.width = width;
        textures[1].image.height = height;
        textures[1].image.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textures[1].image.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        createImage(globals, textures[1].image);
        transitionImageLayout(globals, textures[1].image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(globals, stagingBuffer, textures[1].image);
        destroyBuffer(globals, stagingBuffer);
        transitionImageLayout(globals, textures[1].image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        createImageView(globals, textures[1].image);
        createSampler(globals, textures[1].sampler);
    }
}

void Boxes::createMaterials()
{
    materials.resize(2);
    {
        materials[0].ambient = glm::vec3(1.f, 0.5f, 0.31f);
        materials[0].diffuse = glm::vec3(1.f, 0.5f, 0.31f);
        materials[0].specular = glm::vec3(0.5f, 0.5f, 0.5f);
        materials[0].shininess = 32.f;
        materials[0].diffuseTexIndex = 0;
        materials[0].specularTexIndex = 1;
    }
    {
        materials[1].ambient = glm::vec3(1.f, 1.f, 1.f);
        materials[1].diffuse = glm::vec3(1.f, 1.f, 1.f);
        materials[1].specular = glm::vec3(1.f, 1.f, 1.f);
        materials[1].shininess = 32.f;
        materials[1].diffuseTexIndex = 0;
        materials[1].specularTexIndex = 1;
    }
}

void Boxes::createRenderObjects()
{
    renderObjects.resize(14);
    {
        renderObjects[0].world = glm::scale(glm::mat4(1.f), glm::vec3(0.5f));
        renderObjects[0].texTransform = glm::mat4(1.f);
        renderObjects[0].meshIndex = 0;
        renderObjects[0].submeshIndex = 0;
        renderObjects[0].materialIndex = 0;

        renderObjects[1].world = glm::translate(glm::mat4(1.f), glm::vec3(2.f,  5.f, -15.f));
        renderObjects[1].world = glm::scale(renderObjects[1].world, glm::vec3(0.5f));
        renderObjects[1].texTransform = glm::mat4(1.f);
        renderObjects[1].meshIndex = 0;
        renderObjects[1].submeshIndex = 0;
        renderObjects[1].materialIndex = 0;

        renderObjects[2].world = glm::translate(glm::mat4(1.f), glm::vec3(-1.5f, -2.2f, -2.5f));
        renderObjects[2].world = glm::scale(renderObjects[2].world, glm::vec3(0.5f));
        renderObjects[2].texTransform = glm::mat4(1.f);
        renderObjects[2].meshIndex = 0;
        renderObjects[2].submeshIndex = 0;
        renderObjects[2].materialIndex = 0;

        renderObjects[3].world = glm::translate(glm::mat4(1.f), glm::vec3(-3.8f, -2.f, -12.3f));
        renderObjects[3].world = glm::scale(renderObjects[3].world, glm::vec3(0.5f));
        renderObjects[3].texTransform = glm::mat4(1.f);
        renderObjects[3].meshIndex = 0;
        renderObjects[3].submeshIndex = 0;
        renderObjects[3].materialIndex = 0;

        renderObjects[4].world = glm::translate(glm::mat4(1.f), glm::vec3(2.4f, -0.4f, -3.5f));
        renderObjects[4].world = glm::scale(renderObjects[4].world, glm::vec3(0.5f));
        renderObjects[4].texTransform = glm::mat4(1.f);
        renderObjects[4].meshIndex = 0;
        renderObjects[4].submeshIndex = 0;
        renderObjects[4].materialIndex = 0;

        renderObjects[5].world = glm::translate(glm::mat4(1.f), glm::vec3(-1.7f,  3.f, -7.5f));
        renderObjects[5].world = glm::scale(renderObjects[5].world, glm::vec3(0.5f));
        renderObjects[5].texTransform = glm::mat4(1.f);
        renderObjects[5].meshIndex = 0;
        renderObjects[5].submeshIndex = 0;
        renderObjects[5].materialIndex = 0;

        renderObjects[6].world = glm::translate(glm::mat4(1.f), glm::vec3(1.3f, -2.f, -2.5f));
        renderObjects[6].world = glm::scale(renderObjects[6].world, glm::vec3(0.5f));
        renderObjects[6].texTransform = glm::mat4(1.f);
        renderObjects[6].meshIndex = 0;
        renderObjects[6].submeshIndex = 0;
        renderObjects[6].materialIndex = 0;

        renderObjects[7].world = glm::translate(glm::mat4(1.f), glm::vec3(1.5f,  2.f, -2.5f));
        renderObjects[7].world = glm::scale(renderObjects[7].world, glm::vec3(0.5f));
        renderObjects[7].texTransform = glm::mat4(1.f);
        renderObjects[7].meshIndex = 0;
        renderObjects[7].submeshIndex = 0;
        renderObjects[7].materialIndex = 0;

        renderObjects[8].world = glm::translate(glm::mat4(1.f), glm::vec3(1.5f,  0.2f, -1.5f));
        renderObjects[8].world = glm::scale(renderObjects[8].world, glm::vec3(0.5f));
        renderObjects[8].texTransform = glm::mat4(1.f);
        renderObjects[8].meshIndex = 0;
        renderObjects[8].submeshIndex = 0;
        renderObjects[8].materialIndex = 0;

        renderObjects[9].world = glm::translate(glm::mat4(1.f), glm::vec3(-1.3f,  1.f, -1.5f));
        renderObjects[9].world = glm::scale(renderObjects[9].world, glm::vec3(0.5f));
        renderObjects[9].texTransform = glm::mat4(1.f);
        renderObjects[9].meshIndex = 0;
        renderObjects[9].submeshIndex = 0;
        renderObjects[9].materialIndex = 0;
    }
    {
        renderObjects[10].world = glm::translate(glm::mat4(1.f), pointLightPositions[0]);
        renderObjects[10].world = glm::scale(renderObjects[10].world, glm::vec3(0.1f));
        renderObjects[10].texTransform = glm::mat4(1.f);
        renderObjects[10].meshIndex = 0;
        renderObjects[10].submeshIndex = 1;
        renderObjects[10].materialIndex = 1;
    }
    {
        renderObjects[11].world = glm::translate(glm::mat4(1.f), pointLightPositions[1]);
        renderObjects[11].world = glm::scale(renderObjects[11].world, glm::vec3(0.1f));
        renderObjects[11].texTransform = glm::mat4(1.f);
        renderObjects[11].meshIndex = 0;
        renderObjects[11].submeshIndex = 1;
        renderObjects[11].materialIndex = 1;
    }
    {
        renderObjects[12].world = glm::translate(glm::mat4(1.f), pointLightPositions[2]);
        renderObjects[12].world = glm::scale(renderObjects[12].world, glm::vec3(0.1f));
        renderObjects[12].texTransform = glm::mat4(1.f);
        renderObjects[12].meshIndex = 0;
        renderObjects[12].submeshIndex = 1;
        renderObjects[12].materialIndex = 1;
    }
    {
        renderObjects[13].world = glm::translate(glm::mat4(1.f), pointLightPositions[3]);
        renderObjects[13].world = glm::scale(renderObjects[13].world, glm::vec3(0.1f));
        renderObjects[13].texTransform = glm::mat4(1.f);
        renderObjects[13].meshIndex = 0;
        renderObjects[13].submeshIndex = 1;
        renderObjects[13].materialIndex = 1;
    }
}

void Boxes::createLights()
{
    {
        lights.direct.dir = glm::vec3(-0.2f, -1.f, -0.3f);
        lights.direct.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
        lights.direct.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
        lights.direct.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    }
    {
        lights.spot.pos = camera.pos;
        lights.spot.dir = camera.target;
        lights.spot.ambient = glm::vec3(0.f, 0.f, 0.f);
        lights.spot.diffuse = glm::vec3(1.f, 1.f, 1.f);
        lights.spot.specular = glm::vec3(1.f, 1.f, 1.f);
        lights.spot.cutOff = glm::cos(glm::radians(12.5f));
        lights.spot.outerCutOff = glm::cos(glm::radians(15.0f));
        lights.spot.constAtt = 1.f;
        lights.spot.linearAtt = 0.09f;
        lights.spot.quadAtt = 0.032f;
    }
    {
        lights.points.resize(4);
        {
            lights.points[0].pos = pointLightPositions[0];
            lights.points[0].ambient = glm::vec3(0.05f, 0.05f, 0.05f);
            lights.points[0].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
            lights.points[0].specular = glm::vec3(1.f, 1.f, 1.f);
            lights.points[0].constAtt = 1.f;
            lights.points[0].linearAtt = 0.09f;
            lights.points[0].quadAtt = 0.032f;

            lights.points[1].pos = pointLightPositions[1];
            lights.points[1].ambient = glm::vec3(0.05f, 0.05f, 0.05f);
            lights.points[1].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
            lights.points[1].specular = glm::vec3(1.f, 1.f, 1.f);
            lights.points[1].constAtt = 1.f;
            lights.points[1].linearAtt = 0.09f;
            lights.points[1].quadAtt = 0.032f;

            lights.points[2].pos = pointLightPositions[2];
            lights.points[2].ambient = glm::vec3(0.05f, 0.05f, 0.05f);
            lights.points[2].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
            lights.points[2].specular = glm::vec3(1.f, 1.f, 1.f);
            lights.points[2].constAtt = 1.f;
            lights.points[2].linearAtt = 0.09f;
            lights.points[2].quadAtt = 0.032f;

            lights.points[3].pos = pointLightPositions[3];
            lights.points[3].ambient = glm::vec3(0.05f, 0.05f, 0.05f);
            lights.points[3].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
            lights.points[3].specular = glm::vec3(1.f, 1.f, 1.f);
            lights.points[3].constAtt = 1.f;
            lights.points[3].linearAtt = 0.09f;
            lights.points[3].quadAtt = 0.032f;
        }
    }
}

void Boxes::createFrameResources()
{
    frameResources.resize(framesInFlight);
    for (u32 i = 0; i < frameResources.size(); ++i) {
        {
            frameResources[i].passBuffer.size = sizeof(Pass);
            frameResources[i].passBuffer.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            frameResources[i].passBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            createBuffer(globals, frameResources[i].passBuffer);
            THROW_IF_FAILED(
                vkMapMemory(globals.device.handle, frameResources[i].passBuffer.memory, 0, frameResources[i].passBuffer.size, 0, &frameResources[i].passBuffer.mapped),
                __FILE__, __LINE__,
                "Failed to map memory");
        }
        {
            Buffer stagingBuffer;
            stagingBuffer.size = sizeof(lights.direct);
            stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            stagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            createBuffer(globals, stagingBuffer);
            THROW_IF_FAILED(
                vkMapMemory(globals.device.handle, stagingBuffer.memory, 0, stagingBuffer.size, 0, &stagingBuffer.mapped),
                __FILE__, __LINE__,
                "Failed to map memory");
            memcpy(stagingBuffer.mapped, &lights.direct, stagingBuffer.size);
            vkUnmapMemory(globals.device.handle, stagingBuffer.memory);

            frameResources[i].dirLightBuffer.size = stagingBuffer.size;
            frameResources[i].dirLightBuffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            frameResources[i].dirLightBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            createBuffer(globals, frameResources[i].dirLightBuffer);
            copyBuffer(globals, stagingBuffer, frameResources[i].dirLightBuffer);
            destroyBuffer(globals, stagingBuffer);
        }
        {
            frameResources[i].spotLightBuffer.size = sizeof(lights.spot);
            frameResources[i].spotLightBuffer.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            frameResources[i].spotLightBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            createBuffer(globals, frameResources[i].spotLightBuffer);
            THROW_IF_FAILED(
                vkMapMemory(globals.device.handle, frameResources[i].spotLightBuffer.memory, 0, frameResources[i].spotLightBuffer.size, 0, &frameResources[i].spotLightBuffer.mapped),
                __FILE__, __LINE__,
                "Failed to map memory");
        }
        {
            Buffer stagingBuffer;
            stagingBuffer.size = lights.points.size() * sizeof(lights.points[0]);
            stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            stagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            createBuffer(globals, stagingBuffer);
            THROW_IF_FAILED(
                vkMapMemory(globals.device.handle, stagingBuffer.memory, 0, stagingBuffer.size, 0, &stagingBuffer.mapped),
                __FILE__, __LINE__,
                "Failed to map memory");
            memcpy(stagingBuffer.mapped, lights.points.data(), stagingBuffer.size);
            vkUnmapMemory(globals.device.handle, stagingBuffer.memory);

            frameResources[i].pointLightBuffer.size = stagingBuffer.size;
            frameResources[i].pointLightBuffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            frameResources[i].pointLightBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            createBuffer(globals, frameResources[i].pointLightBuffer);
            copyBuffer(globals, stagingBuffer, frameResources[i].pointLightBuffer);
            destroyBuffer(globals, stagingBuffer);
        }
        {
            Buffer stagingBuffer;
            stagingBuffer.size = materials.size() * sizeof(materials[0]);
            stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            stagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            createBuffer(globals, stagingBuffer);
            THROW_IF_FAILED(
                vkMapMemory(globals.device.handle, stagingBuffer.memory, 0, stagingBuffer.size, 0, &stagingBuffer.mapped),
                __FILE__, __LINE__,
                "Failed to map memory");
            memcpy(stagingBuffer.mapped, materials.data(), stagingBuffer.size);
            vkUnmapMemory(globals.device.handle, stagingBuffer.memory);

            frameResources[i].materialBuffer.size = stagingBuffer.size;
            frameResources[i].materialBuffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            frameResources[i].materialBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            createBuffer(globals, frameResources[i].materialBuffer);
            copyBuffer(globals, stagingBuffer, frameResources[i].materialBuffer);
            destroyBuffer(globals, stagingBuffer);
        }
        {
            Buffer stagingBuffer;
            stagingBuffer.alignment = calculateUniformBufferAlignment(globals, sizeof(renderObjects[0]));
            stagingBuffer.size = renderObjects.size() * stagingBuffer.alignment;
            stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            stagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            createBuffer(globals, stagingBuffer);
            THROW_IF_FAILED(
                vkMapMemory(globals.device.handle, stagingBuffer.memory, 0, stagingBuffer.size, 0, &stagingBuffer.mapped),
                __FILE__, __LINE__,
                "Failed to map memory");
            for (u32 i = 0; i < renderObjects.size(); ++i) {
                memcpy(reinterpret_cast<u8*>(stagingBuffer.mapped) + (i * stagingBuffer.alignment), &renderObjects[i], sizeof(renderObjects[0]));
            }
            vkUnmapMemory(globals.device.handle, stagingBuffer.memory);

            frameResources[i].renderObjectBuffer.alignment = stagingBuffer.alignment;
            frameResources[i].renderObjectBuffer.size = stagingBuffer.size;
            frameResources[i].renderObjectBuffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            frameResources[i].renderObjectBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            createBuffer(globals, frameResources[i].renderObjectBuffer);
            copyBuffer(globals, stagingBuffer, frameResources[i].renderObjectBuffer);
            destroyBuffer(globals, stagingBuffer);
        }
    }
}

void Boxes::createResourceDescriptors()
{
    resourceDescriptors.resize(3);
    {
        std::vector<VkDescriptorPoolSize> poolSizes(4);
        poolSizes[0] = Initializer::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, framesInFlight);
        poolSizes[1] = Initializer::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, framesInFlight);
        poolSizes[2] = Initializer::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, framesInFlight);
        poolSizes[3] = Initializer::descriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, framesInFlight);
        auto descriptorPoolCreateInfo = Initializer::descriptorPoolCreateInfo(framesInFlight, poolSizes);
        THROW_IF_FAILED(
            vkCreateDescriptorPool(globals.device.handle, &descriptorPoolCreateInfo, globals.allocator, &resourceDescriptors[0].pool),
            __FILE__, __LINE__,
            "Failed to create descriptor pool");

        std::vector<VkDescriptorSetLayoutBinding> bindings(4);
        bindings[0] = Initializer::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
        bindings[1] = Initializer::descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
        bindings[2] = Initializer::descriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
        bindings[3] = Initializer::descriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
        auto descriptorSetLayoutCreateInfo = Initializer::descriptorSetLayoutCreateInfo(bindings);
        THROW_IF_FAILED(
            vkCreateDescriptorSetLayout(globals.device.handle, &descriptorSetLayoutCreateInfo, globals.allocator, &resourceDescriptors[0].setLayout),
            __FILE__, __LINE__,
            "Failed to create descriptor set layout");

        std::vector<VkDescriptorSetLayout> setLayouts(framesInFlight, resourceDescriptors[0].setLayout);
        auto descriptorSetAllocateInfo = Initializer::descriptorSetAllocateInfo(resourceDescriptors[0].pool, framesInFlight, setLayouts);
        resourceDescriptors[0].handles.resize(framesInFlight);
        THROW_IF_FAILED(
            vkAllocateDescriptorSets(globals.device.handle, &descriptorSetAllocateInfo, resourceDescriptors[0].handles.data()),
            __FILE__, __LINE__,
            "Failed to allocate descriptor sets");

        for (u32 i = 0; i < framesInFlight; ++i) {
            std::vector<VkDescriptorBufferInfo> uniformBufferDescriptors(3);
            uniformBufferDescriptors[0] = Initializer::descriptorBufferInfo(frameResources[i].passBuffer.handle, 0);
            uniformBufferDescriptors[1] = Initializer::descriptorBufferInfo(frameResources[i].dirLightBuffer.handle, 0);
            uniformBufferDescriptors[2] = Initializer::descriptorBufferInfo(frameResources[i].spotLightBuffer.handle, 0);
            std::vector<VkDescriptorBufferInfo> storageBufferDescriptors(1);
            storageBufferDescriptors[0] = Initializer::descriptorBufferInfo(frameResources[i].pointLightBuffer.handle, 0);
            std::vector<VkWriteDescriptorSet> descriptorWrites(2);
            descriptorWrites[0] = Initializer::writeDescriptorSet(
                resourceDescriptors[0].handles[i],
                0, 0, uniformBufferDescriptors.size(),
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                nullptr, uniformBufferDescriptors.data(), nullptr);
            descriptorWrites[1] = Initializer::writeDescriptorSet(
                resourceDescriptors[0].handles[i],
                3, 0, storageBufferDescriptors.size(),
                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                nullptr, storageBufferDescriptors.data(), nullptr);
            vkUpdateDescriptorSets(globals.device.handle, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        }
    }
    {
        std::vector<VkDescriptorPoolSize> poolSizes(2);
        poolSizes[0] = Initializer::descriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, framesInFlight);
        poolSizes[1] = Initializer::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, framesInFlight * textures.size());
        auto descriptorPoolCreateInfo = Initializer::descriptorPoolCreateInfo(framesInFlight, poolSizes);
        THROW_IF_FAILED(
            vkCreateDescriptorPool(globals.device.handle, &descriptorPoolCreateInfo, globals.allocator, &resourceDescriptors[1].pool),
            __FILE__, __LINE__,
            "Failed to create descriptor pool");

        std::vector<VkDescriptorSetLayoutBinding> bindings(2);
        bindings[0] = Initializer::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        bindings[1] = Initializer::descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, textures.size(), VK_SHADER_STAGE_FRAGMENT_BIT);
        std::vector<VkDescriptorBindingFlags> descriptorBindingFlags(2);
        descriptorBindingFlags[0] = 0;
        descriptorBindingFlags[1] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
        auto descriptorSetLayoutBindingFlagsCreateInfo = Initializer::descriptorSetLayoutBindingFlagsCreateInfo(descriptorBindingFlags);
        auto descriptorSetLayoutCreateInfo = Initializer::descriptorSetLayoutCreateInfo(bindings, &descriptorSetLayoutBindingFlagsCreateInfo);
        THROW_IF_FAILED(
            vkCreateDescriptorSetLayout(globals.device.handle, &descriptorSetLayoutCreateInfo, globals.allocator, &resourceDescriptors[1].setLayout),
            __FILE__, __LINE__,
            "Failed to create descriptor set layout");

        std::vector<u32> descriptorCounts(framesInFlight, textures.size());
        auto descriptorSetVariableDescriptorCountAllocateInfo = Initializer::descriptorSetVariableDescriptorCountAllocateInfo(descriptorCounts);
        std::vector<VkDescriptorSetLayout> setLayouts(framesInFlight, resourceDescriptors[1].setLayout);
        auto descriptorSetAllocateInfo = Initializer::descriptorSetAllocateInfo(
            resourceDescriptors[1].pool,
            framesInFlight,
            setLayouts,
            &descriptorSetVariableDescriptorCountAllocateInfo);
        resourceDescriptors[1].handles.resize(framesInFlight);
        THROW_IF_FAILED(
            vkAllocateDescriptorSets(globals.device.handle, &descriptorSetAllocateInfo, resourceDescriptors[1].handles.data()),
            __FILE__, __LINE__,
            "Failed to allocate descriptor sets");

        for (u32 i = 0; i < framesInFlight; ++i) {
            std::vector<VkDescriptorBufferInfo> bufferDescriptors(1);
            bufferDescriptors[0] = Initializer::descriptorBufferInfo(frameResources[i].materialBuffer.handle, 0);
            std::vector<VkDescriptorImageInfo> imageDescriptors(textures.size());
            for (u32 j = 0; j < imageDescriptors.size(); ++j) {
                imageDescriptors[j].sampler = textures[j].sampler.handle;
                imageDescriptors[j].imageView = textures[j].image.view;
                imageDescriptors[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }
            std::vector<VkWriteDescriptorSet> descriptorWrites(2);
            descriptorWrites[0] = Initializer::writeDescriptorSet(
                resourceDescriptors[1].handles[i],
                0, 0, bufferDescriptors.size(),
                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                nullptr, bufferDescriptors.data(), nullptr);
            descriptorWrites[1] = Initializer::writeDescriptorSet(
                resourceDescriptors[1].handles[i],
                1, 0, imageDescriptors.size(),
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                imageDescriptors.data(), nullptr, nullptr);
            vkUpdateDescriptorSets(globals.device.handle, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        }
    }
    {
        std::vector<VkDescriptorPoolSize> poolSizes(1);
        poolSizes[0] = Initializer::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, framesInFlight);
        auto descriptorPoolCreateInfo = Initializer::descriptorPoolCreateInfo(framesInFlight, poolSizes);
        THROW_IF_FAILED(
            vkCreateDescriptorPool(globals.device.handle, &descriptorPoolCreateInfo, globals.allocator, &resourceDescriptors[2].pool),
            __FILE__, __LINE__,
            "Failed to create descriptor pool");

        std::vector<VkDescriptorSetLayoutBinding> bindings(1);
        bindings[0] = Initializer::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
        auto descriptorSetLayoutCreateInfo = Initializer::descriptorSetLayoutCreateInfo(bindings);
        THROW_IF_FAILED(
            vkCreateDescriptorSetLayout(globals.device.handle, &descriptorSetLayoutCreateInfo, globals.allocator, &resourceDescriptors[2].setLayout),
            __FILE__, __LINE__,
            "Failed to create descriptor set layout");

        std::vector<VkDescriptorSetLayout> setLayouts(framesInFlight, resourceDescriptors[2].setLayout);
        auto descriptorSetAllocateInfo = Initializer::descriptorSetAllocateInfo(resourceDescriptors[2].pool, framesInFlight, setLayouts);
        resourceDescriptors[2].handles.resize(framesInFlight);
        THROW_IF_FAILED(
            vkAllocateDescriptorSets(globals.device.handle, &descriptorSetAllocateInfo, resourceDescriptors[2].handles.data()),
            __FILE__, __LINE__,
            "Failed to allocate descriptor sets");

        for (u32 i = 0; i < framesInFlight; ++i) {
            std::vector<VkDescriptorBufferInfo> bufferDescriptors(1);
            bufferDescriptors[0] = Initializer::descriptorBufferInfo(frameResources[i].renderObjectBuffer.handle, 0, sizeof(renderObjects[0]));
            std::vector<VkWriteDescriptorSet> descriptorWrites(1);
            descriptorWrites[0] = Initializer::writeDescriptorSet(
                resourceDescriptors[2].handles[i],
                0, 0, bufferDescriptors.size(),
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
                nullptr, bufferDescriptors.data(), nullptr);
            vkUpdateDescriptorSets(globals.device.handle, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        }
    }
}

void Boxes::createPushConstantRanges()
{
}

void Boxes::createPipelines()
{
    pipelines.resize(2);
    pipelineLayouts.resize(2);
    {
        std::vector<VkPipelineShaderStageCreateInfo> stages(2);
        VkShaderModule shaderModule[2];
        {
            auto code = loadShaderCode("Boxes/CubeVertex.spv");
            auto shaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(code);
            THROW_IF_FAILED(
                vkCreateShaderModule(globals.device.handle, &shaderModuleCreateInfo, globals.allocator, &shaderModule[0]),
                __FILE__, __LINE__,
                "Failed to create shader module");
            stages[0] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, shaderModule[0]);
        }
        {
            auto code = loadShaderCode("Boxes/CubeFragment.spv");
            auto shaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(code);
            THROW_IF_FAILED(
                vkCreateShaderModule(globals.device.handle, &shaderModuleCreateInfo, globals.allocator, &shaderModule[1]),
                __FILE__, __LINE__,
                "Failed to create shader module");
            stages[1] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, shaderModule[1]);
        }

        std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions(1);
        vertexBindingDescriptions[0] = Initializer::vertexInputBindingDescription(0, sizeof(Vertex));
        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions(3);
        vertexAttributeDescriptions[0] = Initializer::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos));
        vertexAttributeDescriptions[1] = Initializer::vertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal));
        vertexAttributeDescriptions[2] = Initializer::vertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord));
        auto vertexInputState = Initializer::pipelineVertexInputStateCreateInfo(vertexBindingDescriptions, vertexAttributeDescriptions);

        auto inputAssemblyState = Initializer::pipelineInputAssemblyStateCreateInfo();
        auto tessellationState = Initializer::pipelineTessellationStateCreateInfo();

        std::vector<VkViewport> viewports(1);
        viewports[0] = Initializer::viewport(globals.swapchain.extent.width, globals.swapchain.extent.height);
        std::vector<VkRect2D> scissors(1);
        scissors[0] = Initializer::scissor(globals.swapchain.extent.width, globals.swapchain.extent.height);
        auto viewportState = Initializer::pipelineViewportStateCreateInfo(viewports, scissors);

        auto rasterizationState = Initializer::pipelineRasterizationStateCreateInfo();
        auto multisampleState = Initializer::pipelineMultisampleStateCreateInfo();
        auto depthStencilState = Initializer::pipelineDepthStencilStateCreateInfo();

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates(1);
        colorBlendAttachmentStates[0] = Initializer::pipelineColorBlendAttachmentState();
        auto colorBlendState = Initializer::pipelineColorBlendStateCreateInfo(colorBlendAttachmentStates);

        std::vector<VkDynamicState> dynamicStates(2);
        dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
        auto dynamicState = Initializer::pipelineDynamicStateCreateInfo(dynamicStates);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(3);
        descriptorSetLayouts[0] = resourceDescriptors[0].setLayout;
        descriptorSetLayouts[1] = resourceDescriptors[1].setLayout;
        descriptorSetLayouts[2] = resourceDescriptors[2].setLayout;
        auto pipelineLayoutCreateInfo = Initializer::pipelineLayoutCreateInfo(descriptorSetLayouts, pushConstantRanges);
        THROW_IF_FAILED(
            vkCreatePipelineLayout(globals.device.handle, &pipelineLayoutCreateInfo, globals.allocator, &pipelineLayouts[0]),
            __FILE__, __LINE__,
            "Failed to create pipeline layout");

        auto createInfo = Initializer::graphicsPipelineCreateInfo(
            stages,
            &vertexInputState,
            &inputAssemblyState,
            &tessellationState,
            &viewportState,
            &rasterizationState,
            &multisampleState,
            &depthStencilState,
            &colorBlendState,
            &dynamicState,
            pipelineLayouts[0],
            globals.renderPass);
        THROW_IF_FAILED(
            vkCreateGraphicsPipelines(globals.device.handle, VK_NULL_HANDLE, 1, &createInfo, globals.allocator, &pipelines[0]),
            __FILE__, __LINE__,
            "Failed to create graphics pipeline");

        vkDestroyShaderModule(globals.device.handle, shaderModule[0], globals.allocator);
        vkDestroyShaderModule(globals.device.handle, shaderModule[1], globals.allocator);
    }
    {
        std::vector<VkPipelineShaderStageCreateInfo> stages(2);
        VkShaderModule shaderModule[2];
        {
            auto code = loadShaderCode("Boxes/LightCubeVertex.spv");
            auto shaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(code);
            THROW_IF_FAILED(
                vkCreateShaderModule(globals.device.handle, &shaderModuleCreateInfo, globals.allocator, &shaderModule[0]),
                __FILE__, __LINE__,
                "Failed to create shader module");
            stages[0] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, shaderModule[0]);
        }
        {
            auto code = loadShaderCode("Boxes/LightCubeFragment.spv");
            auto shaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(code);
            THROW_IF_FAILED(
                vkCreateShaderModule(globals.device.handle, &shaderModuleCreateInfo, globals.allocator, &shaderModule[1]),
                __FILE__, __LINE__,
                "Failed to create shader module");
            stages[1] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, shaderModule[1]);
        }

        std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions(1);
        vertexBindingDescriptions[0] = Initializer::vertexInputBindingDescription(0, sizeof(Vertex));
        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions(3);
        vertexAttributeDescriptions[0] = Initializer::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos));
        vertexAttributeDescriptions[1] = Initializer::vertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal));
        vertexAttributeDescriptions[2] = Initializer::vertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord));
        auto vertexInputState = Initializer::pipelineVertexInputStateCreateInfo(vertexBindingDescriptions, vertexAttributeDescriptions);

        auto inputAssemblyState = Initializer::pipelineInputAssemblyStateCreateInfo();
        auto tessellationState = Initializer::pipelineTessellationStateCreateInfo();

        std::vector<VkViewport> viewports(1);
        viewports[0] = Initializer::viewport(globals.swapchain.extent.width, globals.swapchain.extent.height);
        std::vector<VkRect2D> scissors(1);
        scissors[0] = Initializer::scissor(globals.swapchain.extent.width, globals.swapchain.extent.height);
        auto viewportState = Initializer::pipelineViewportStateCreateInfo(viewports, scissors);

        auto rasterizationState = Initializer::pipelineRasterizationStateCreateInfo();
        auto multisampleState = Initializer::pipelineMultisampleStateCreateInfo();
        auto depthStencilState = Initializer::pipelineDepthStencilStateCreateInfo();

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates(1);
        colorBlendAttachmentStates[0] = Initializer::pipelineColorBlendAttachmentState();
        auto colorBlendState = Initializer::pipelineColorBlendStateCreateInfo(colorBlendAttachmentStates);

        std::vector<VkDynamicState> dynamicStates(2);
        dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
        auto dynamicState = Initializer::pipelineDynamicStateCreateInfo(dynamicStates);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts(3);
        descriptorSetLayouts[0] = resourceDescriptors[0].setLayout;
        descriptorSetLayouts[1] = resourceDescriptors[1].setLayout;
        descriptorSetLayouts[2] = resourceDescriptors[2].setLayout;
        auto pipelineLayoutCreateInfo = Initializer::pipelineLayoutCreateInfo(descriptorSetLayouts, pushConstantRanges);
        THROW_IF_FAILED(
            vkCreatePipelineLayout(globals.device.handle, &pipelineLayoutCreateInfo, globals.allocator, &pipelineLayouts[1]),
            __FILE__, __LINE__,
            "Failed to create pipeline layout");

        auto createInfo = Initializer::graphicsPipelineCreateInfo(
            stages,
            &vertexInputState,
            &inputAssemblyState,
            &tessellationState,
            &viewportState,
            &rasterizationState,
            &multisampleState,
            &depthStencilState,
            &colorBlendState,
            &dynamicState,
            pipelineLayouts[1],
            globals.renderPass);
        THROW_IF_FAILED(
            vkCreateGraphicsPipelines(globals.device.handle, VK_NULL_HANDLE, 1, &createInfo, globals.allocator, &pipelines[1]),
            __FILE__, __LINE__,
            "Failed to create graphics pipeline");

        vkDestroyShaderModule(globals.device.handle, shaderModule[0], globals.allocator);
        vkDestroyShaderModule(globals.device.handle, shaderModule[1], globals.allocator);
    }
}

void Boxes::recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex, ImDrawData* draw_data)
{
    auto commandBufferBeginInfo = Initializer::commandBufferBeginInfo();
    THROW_IF_FAILED(
        vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo),
        __FILE__, __LINE__,
        "Failed to begin command buffer");

    VkClearValue clearColor = {};
    clearColor.color = {{ 0.f, 0.f, 0.f, 1.f }};
    VkClearValue clearDepth = {};
    clearDepth.depthStencil = { 1.f, 0 };
    std::vector<VkClearValue> clearValues(2);
    clearValues[0] = clearColor;
    clearValues[1] = clearDepth;

    VkRect2D renderArea = { { 0, 0 }, globals.swapchain.extent };
    auto renderPassBeginInfo = Initializer::renderPassBeginInfo(
        globals.renderPass,
        globals.swapchain.framebuffers[imageIndex],
        renderArea,
        clearValues);
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    auto viewport = Initializer::viewport(globals.swapchain.extent.width, globals.swapchain.extent.height);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    auto scissor = Initializer::scissor(globals.swapchain.extent.width, globals.swapchain.extent.height);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    std::vector<VkDeviceSize> offsets(1);
    offsets[0] = 0;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &meshes[0].vertexBuffer.handle, offsets.data());
    vkCmdBindIndexBuffer(commandBuffer, meshes[0].indexBuffer.handle, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayouts[0],
        0, 1, &resourceDescriptors[0].handles[frameIndex],
        0, nullptr);

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayouts[0],
        1, 1, &resourceDescriptors[1].handles[frameIndex],
        0, nullptr);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[0]);

    for (u32 i = 0; i < 10; ++i) {
        u32 dynamicOffset = i * frameResources[frameIndex].renderObjectBuffer.alignment;

        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayouts[0],
            2, 1, &resourceDescriptors[2].handles[frameIndex],
            1, &dynamicOffset);

        vkCmdDrawIndexed(
            commandBuffer,
            meshes[0].submeshes[renderObjects[i].submeshIndex].indexCount, 1,
            meshes[0].submeshes[renderObjects[i].submeshIndex].firstIndex,
            meshes[0].submeshes[renderObjects[i].submeshIndex].vertexOffset, 0);
    }

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[1]);

    // vkCmdBindDescriptorSets(
    //     commandBuffer,
    //     VK_PIPELINE_BIND_POINT_GRAPHICS,
    //     pipelineLayouts[1],
    //     0, 1, &resourceDescriptors[0].sets[frameIndex],
    //     0, nullptr);

    // vkCmdBindDescriptorSets(
    //     commandBuffer,
    //     VK_PIPELINE_BIND_POINT_GRAPHICS,
    //     pipelineLayouts[1],
    //     1, 1, &resourceDescriptors[1].sets[frameIndex],
    //     0, nullptr);

    for (u32 i = 10; i < 14; ++i) {
        u32 dynamicOffset = i * frameResources[frameIndex].renderObjectBuffer.alignment;

        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayouts[1],
            2, 1, &resourceDescriptors[2].handles[frameIndex],
            1, &dynamicOffset);

        vkCmdDrawIndexed(
            commandBuffer,
            meshes[0].submeshes[renderObjects[i].submeshIndex].indexCount, 1,
            meshes[0].submeshes[renderObjects[i].submeshIndex].firstIndex,
            meshes[0].submeshes[renderObjects[i].submeshIndex].vertexOffset, 0);
    }

    ImGui_ImplVulkan_RenderDrawData(draw_data, globals.graphicsCommandBuffer.buffers[frameIndex]);

    vkCmdEndRenderPass(commandBuffer);
    THROW_IF_FAILED(
        vkEndCommandBuffer(commandBuffer),
        __FILE__, __LINE__,
        "Failed to end command buffer");
}

void Boxes::updateFrameResources(u32 frameIndex)
{
    {
        Pass pass = {};
        pass.view = camera.matrices.view;
        pass.proj = camera.matrices.proj;
        pass.viewPos = camera.pos;
        memcpy(frameResources[frameIndex].passBuffer.mapped, &pass, sizeof(pass));

        SpotLight spotLight = {};
        spotLight.pos = camera.pos;
        spotLight.dir = camera.target;
        spotLight.ambient = glm::vec3(0.f, 0.f, 0.f);
        spotLight.diffuse = glm::vec3(1.f, 1.f, 1.f);
        spotLight.specular = glm::vec3(1.f, 1.f, 1.f);
        spotLight.cutOff = glm::cos(glm::radians(12.5f));
        spotLight.outerCutOff = glm::cos(glm::radians(15.0f));
        spotLight.constAtt = 1.f;
        spotLight.linearAtt = 0.09f;
        spotLight.quadAtt = 0.032f;
        memcpy(frameResources[frameIndex].spotLightBuffer.mapped, &spotLight, sizeof(spotLight));
    }
    {
        // materials
    }
    {
        // render objects
    }
}

void Boxes::destroyPipelines()
{
    for (u32 i = 0; i < pipelineLayouts.size(); ++i) {
        vkDestroyPipelineLayout(globals.device.handle, pipelineLayouts[i], globals.allocator);
    }

    for (u32 i = 0; i < pipelines.size(); ++i) {
        vkDestroyPipeline(globals.device.handle, pipelines[i], globals.allocator);
    }
}

void Boxes::destroyPushConstantRanges()
{
}

void Boxes::destroyResourceDescriptors()
{
    for (u32 i = 0; i < resourceDescriptors.size(); ++i) {
        vkDestroyDescriptorSetLayout(globals.device.handle, resourceDescriptors[i].setLayout, globals.allocator);
    }

    for (u32 i = 0; i < resourceDescriptors.size(); ++i) {
        vkDestroyDescriptorPool(globals.device.handle, resourceDescriptors[i].pool, globals.allocator);
    }
}

void Boxes::destroyFrameResources()
{
    for (u32 i = 0; i < frameResources.size(); ++i) {
        vkUnmapMemory(globals.device.handle, frameResources[i].passBuffer.memory);
        destroyBuffer(globals, frameResources[i].passBuffer);
        destroyBuffer(globals, frameResources[i].dirLightBuffer);
        vkUnmapMemory(globals.device.handle, frameResources[i].spotLightBuffer.memory);
        destroyBuffer(globals, frameResources[i].spotLightBuffer);
        destroyBuffer(globals, frameResources[i].pointLightBuffer);
        destroyBuffer(globals, frameResources[i].materialBuffer);
        destroyBuffer(globals, frameResources[i].renderObjectBuffer);
    }
}

void Boxes::destroyTextures()
{
    for (u32 i = 0; i < textures.size(); ++i) {
        vkDestroySampler(globals.device.handle, textures[i].sampler.handle, globals.allocator);
        vkDestroyImageView(globals.device.handle, textures[i].image.view, globals.allocator);
        destroyImage(globals, textures[i].image);
    }
}

void Boxes::destroyMeshes()
{
    for (u32 i = 0; i < meshes.size(); ++i) {
        destroyBuffer(globals, meshes[i].indexBuffer);
        destroyBuffer(globals, meshes[i].vertexBuffer);
    }
}

std::unique_ptr<SampleBase> createSample()
{
    return std::make_unique<Boxes>(1600, 900, "Boxes");
}
