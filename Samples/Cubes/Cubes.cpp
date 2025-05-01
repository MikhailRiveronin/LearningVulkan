#include "Boilerplate/Entry.h"
#include "Boilerplate/EventManager.h"
#include "Boilerplate/ProceduralMeshes/Box.h"
#include "Boilerplate/Structures.h"
#include "Boilerplate/Utils.h"
#include "Boilerplate/SampleBase.h"
#include "Boilerplate/Application.h"
#include "Boilerplate/Initializer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

class Cubes : public SampleBase {
public:
    Cubes(u32 width, u32 height, std::string const& name);

private:
    std::vector<Mesh> meshes;
    std::vector<Texture> textures;
    std::vector<Material> materials;
    std::vector<RenderObject> renderObjects;
    std::vector<FrameResource> frameResources;
    std::vector<ResourceDescriptor> resourceDescriptors;
    std::vector<VkPushConstantRange> pushConstantRanges;
    std::vector<VkPipelineLayout> pipelineLayouts;
    std::vector<VkPipeline> pipelines;

    Camera camera;
    glm::vec3 lightPos = glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 lightColor = glm::vec3(1.f);
    glm::vec3 diffuseColor;
    glm::vec3 ambientColor;

    void createMeshes() override;
    void createTextures() override;
    void createMaterials() override;
    void createRenderObjects() override;
    void createFrameResources() override;
    void createResourceDescriptors() override;
    void createPushConstantRanges() override;
    void createPipelines() override;

    void updateFrameResources(u32 frameIndex) override;
    void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex) override;

    void destroyPipelines() override;
    void destroyPushConstantRanges() override;
    void destroyResourceDescriptors() override;
    void destroyFrameResources() override;
    void destroyTextures() override;
    void destroyMeshes() override;
};

Cubes::Cubes(u32 width, u32 height, std::string const& name) :
    SampleBase(width, height, name),
    camera(globals, glm::vec3(0.f, -0.f, 4.f))
{
    EventManager::subscribe(EventType::LEFT_BUTTON_DOWN, &camera);
    EventManager::subscribe(EventType::MOUSE_MOVE, &camera);
    EventManager::subscribe(EventType::KEY_DOWN, &camera);
    EventManager::subscribe(EventType::WINDOW_RESIZE, this);

    diffuseColor = lightColor * glm::vec3(0.5f);
    ambientColor = diffuseColor * glm::vec3(0.2f);
}

void Cubes::createMeshes()
{
    meshes.resize(1);
    {
        std::vector<Vertex> vertices;
        std::vector<u16> indices;
        meshes[0].submeshes.resize(1);
        {
            vertices.insert(vertices.end(), Box::vertices.begin(), Box::vertices.end());
            indices.insert(indices.end(), Box::indices.begin(), Box::indices.end());

            meshes[0].submeshes[0].indexCount = indices.size();
            meshes[0].submeshes[0].firstIndex = 0;
            meshes[0].submeshes[0].vertexOffset = 0;
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

void Cubes::createTextures()
{
    textures.resize(1);
    {
        int width;
        int height;
        int channels;
        stbi_uc* pixels = stbi_load("Textures/texture.jpg", &width, &height, &channels, STBI_rgb_alpha);
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
}

void Cubes::createMaterials()
{
    materials.resize(2);
    {
        materials[0].ambient = glm::vec3(1.f, 0.5f, 0.31f);
        materials[0].diffuse = glm::vec3(1.f, 0.5f, 0.31f);
        materials[0].specular = glm::vec3(0.5f, 0.5f, 0.5f);
        materials[0].shininess = 32.f;
        materials[0].texIndex = 0;
    }
    {
        materials[1].ambient = glm::vec3(1.f, 1.f, 1.f);
        materials[1].diffuse = glm::vec3(1.f, 1.f, 1.f);
        materials[1].specular = glm::vec3(1.f, 1.f, 1.f);
        materials[1].shininess = 32.f;
        materials[1].texIndex = 0;
    }
}

void Cubes::createRenderObjects()
{
    renderObjects.resize(2);
    {
        renderObjects[0].world = glm::scale(glm::mat4(1.f), glm::vec3(0.5f));
        renderObjects[0].texTransform = glm::mat4(1.f);
        renderObjects[0].meshIndex = 0;
        renderObjects[0].submeshIndex = 0;
        renderObjects[0].materialIndex = 0;
    }
    {
        glm::mat4 dlfdlf = glm::translate(glm::mat4(1.f), lightPos);
        renderObjects[1].world = dlfdlf;
        renderObjects[1].world = glm::scale(renderObjects[1].world, glm::vec3(0.1f));
        renderObjects[1].texTransform = glm::mat4(1.f);
        renderObjects[1].meshIndex = 0;
        renderObjects[1].submeshIndex = 0;
        renderObjects[1].materialIndex = 1;
    }
}

void Cubes::createFrameResources()
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

void Cubes::createResourceDescriptors()
{
    resourceDescriptors.resize(3);
    {
        std::vector<VkDescriptorPoolSize> poolSizes(1);
        poolSizes[0] = Initializer::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, framesInFlight);
        auto descriptorPoolCreateInfo = Initializer::descriptorPoolCreateInfo(framesInFlight, poolSizes);
        THROW_IF_FAILED(
            vkCreateDescriptorPool(globals.device.handle, &descriptorPoolCreateInfo, globals.allocator, &resourceDescriptors[0].pool),
            __FILE__, __LINE__,
            "Failed to create descriptor pool");

        std::vector<VkDescriptorSetLayoutBinding> bindings(1);
        bindings[0] = Initializer::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
        auto descriptorSetLayoutCreateInfo = Initializer::descriptorSetLayoutCreateInfo(bindings);
        THROW_IF_FAILED(
            vkCreateDescriptorSetLayout(globals.device.handle, &descriptorSetLayoutCreateInfo, globals.allocator, &resourceDescriptors[0].setLayout),
            __FILE__, __LINE__,
            "Failed to create descriptor set layout");

        std::vector<VkDescriptorSetLayout> setLayouts(framesInFlight, resourceDescriptors[0].setLayout);
        auto descriptorSetAllocateInfo = Initializer::descriptorSetAllocateInfo(resourceDescriptors[0].pool, framesInFlight, setLayouts);
        resourceDescriptors[0].sets.resize(framesInFlight);
        THROW_IF_FAILED(
            vkAllocateDescriptorSets(globals.device.handle, &descriptorSetAllocateInfo, resourceDescriptors[0].sets.data()),
            __FILE__, __LINE__,
            "Failed to allocate descriptor sets");

        for (u32 i = 0; i < framesInFlight; ++i) {
            std::vector<VkDescriptorBufferInfo> bufferDescriptors(1);
            bufferDescriptors[0] = Initializer::descriptorBufferInfo(frameResources[i].passBuffer.handle, 0);
            std::vector<VkWriteDescriptorSet> descriptorWrites(1);
            descriptorWrites[0] = Initializer::writeDescriptorSet(
                resourceDescriptors[0].sets[i],
                0, 0, bufferDescriptors.size(),
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                nullptr, bufferDescriptors.data(), nullptr);
            vkUpdateDescriptorSets(globals.device.handle, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        }
    }
    {
        std::vector<VkDescriptorPoolSize> poolSizes(1);
        poolSizes[0] = Initializer::descriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, framesInFlight);
        // poolSizes[1] = Initializer::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, framesInFlight * textures.size());
        auto descriptorPoolCreateInfo = Initializer::descriptorPoolCreateInfo(framesInFlight, poolSizes);
        THROW_IF_FAILED(
            vkCreateDescriptorPool(globals.device.handle, &descriptorPoolCreateInfo, globals.allocator, &resourceDescriptors[1].pool),
            __FILE__, __LINE__,
            "Failed to create descriptor pool");

        std::vector<VkDescriptorSetLayoutBinding> bindings(1);
        bindings[0] = Initializer::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        // bindings[1] = Initializer::descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, textures.size(), VK_SHADER_STAGE_FRAGMENT_BIT);
        // std::vector<VkDescriptorBindingFlags> descriptorBindingFlags(2);
        // descriptorBindingFlags[0] = 0;
        // descriptorBindingFlags[1] = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
        // auto descriptorSetLayoutBindingFlagsCreateInfo = Initializer::descriptorSetLayoutBindingFlagsCreateInfo(descriptorBindingFlags);
        auto descriptorSetLayoutCreateInfo = Initializer::descriptorSetLayoutCreateInfo(bindings/*, &descriptorSetLayoutBindingFlagsCreateInfo*/);
        THROW_IF_FAILED(
            vkCreateDescriptorSetLayout(globals.device.handle, &descriptorSetLayoutCreateInfo, globals.allocator, &resourceDescriptors[1].setLayout),
            __FILE__, __LINE__,
            "Failed to create descriptor set layout");

        std::vector<VkDescriptorSetLayout> setLayouts(framesInFlight, resourceDescriptors[1].setLayout);
        auto descriptorSetAllocateInfo = Initializer::descriptorSetAllocateInfo(resourceDescriptors[1].pool, framesInFlight, setLayouts);
        resourceDescriptors[1].sets.resize(framesInFlight);
        THROW_IF_FAILED(
            vkAllocateDescriptorSets(globals.device.handle, &descriptorSetAllocateInfo, resourceDescriptors[1].sets.data()),
            __FILE__, __LINE__,
            "Failed to allocate descriptor sets");

        for (u32 i = 0; i < framesInFlight; ++i) {
            std::vector<VkDescriptorBufferInfo> bufferDescriptors(1);
            bufferDescriptors[0] = Initializer::descriptorBufferInfo(frameResources[i].materialBuffer.handle, 0);
            // std::vector<VkDescriptorImageInfo> imageDescriptors(textures.size());
            // for (u32 j = 0; j < imageDescriptors.size(); ++j) {
            //     imageDescriptors[j].sampler = textures[j].sampler.handle;
            //     imageDescriptors[j].imageView = textures[j].image.view;
            //     imageDescriptors[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            // }
            std::vector<VkWriteDescriptorSet> descriptorWrites(1);
            descriptorWrites[0] = Initializer::writeDescriptorSet(
                resourceDescriptors[1].sets[i],
                0, 0, bufferDescriptors.size(),
                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                nullptr, bufferDescriptors.data(), nullptr);
            // descriptorWrites[1] = Initializer::writeDescriptorSet(
            //     resourceDescriptors[1].sets[i],
            //     1, 0, imageDescriptors.size(),
            //     VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            //     imageDescriptors.data(), nullptr, nullptr);
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
        resourceDescriptors[2].sets.resize(framesInFlight);
        THROW_IF_FAILED(
            vkAllocateDescriptorSets(globals.device.handle, &descriptorSetAllocateInfo, resourceDescriptors[2].sets.data()),
            __FILE__, __LINE__,
            "Failed to allocate descriptor sets");

        for (u32 i = 0; i < framesInFlight; ++i) {
            std::vector<VkDescriptorBufferInfo> bufferDescriptors(1);
            bufferDescriptors[0] = Initializer::descriptorBufferInfo(frameResources[i].renderObjectBuffer.handle, 0, sizeof(renderObjects[0]));
            std::vector<VkWriteDescriptorSet> descriptorWrites(1);
            descriptorWrites[0] = Initializer::writeDescriptorSet(
                resourceDescriptors[2].sets[i],
                0, 0, bufferDescriptors.size(),
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
                nullptr, bufferDescriptors.data(), nullptr);
            vkUpdateDescriptorSets(globals.device.handle, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        }
    }
}

void Cubes::createPushConstantRanges()
{
}

void Cubes::createPipelines()
{
    pipelines.resize(2);
    pipelineLayouts.resize(2);
    {
        std::vector<VkPipelineShaderStageCreateInfo> stages(2);
        VkShaderModule shaderModule[2];
        {
            auto code = loadShaderCode("Cubes/CubeVertex.spv");
            auto shaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(code);
            THROW_IF_FAILED(
                vkCreateShaderModule(globals.device.handle, &shaderModuleCreateInfo, globals.allocator, &shaderModule[0]),
                __FILE__, __LINE__,
                "Failed to create shader module");
            stages[0] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, shaderModule[0]);
        }
        {
            auto code = loadShaderCode("Cubes/CubeFragment.spv");
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
            auto code = loadShaderCode("Cubes/CubeVertex.spv");
            auto shaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(code);
            THROW_IF_FAILED(
                vkCreateShaderModule(globals.device.handle, &shaderModuleCreateInfo, globals.allocator, &shaderModule[0]),
                __FILE__, __LINE__,
                "Failed to create shader module");
            stages[0] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, shaderModule[0]);
        }
        {
            auto code = loadShaderCode("Cubes/CubeFragment.spv");
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

void Cubes::recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex)
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

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[0]);

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayouts[0],
        0, 1, &resourceDescriptors[0].sets[frameIndex],
        0, nullptr);

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayouts[0],
        1, 1, &resourceDescriptors[1].sets[frameIndex],
        0, nullptr);

    std::vector<u32> dynamicOffsets(2);
    dynamicOffsets[0] = 0 * frameResources[frameIndex].renderObjectBuffer.alignment;
    dynamicOffsets[1] = 1 * frameResources[frameIndex].renderObjectBuffer.alignment;

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayouts[0],
        2, 1, &resourceDescriptors[2].sets[frameIndex],
        1, &dynamicOffsets[0]);

    vkCmdDrawIndexed(
        commandBuffer,
        meshes[0].submeshes[renderObjects[0].submeshIndex].indexCount, 1,
        meshes[0].submeshes[renderObjects[0].submeshIndex].firstIndex,
        meshes[0].submeshes[renderObjects[0].submeshIndex].vertexOffset, 0);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[1]);

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayouts[1],
        0, 1, &resourceDescriptors[0].sets[frameIndex],
        0, nullptr);

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayouts[1],
        1, 1, &resourceDescriptors[1].sets[frameIndex],
        0, nullptr);

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayouts[1],
        2, 1, &resourceDescriptors[2].sets[frameIndex],
        1, &dynamicOffsets[1]);

    vkCmdDrawIndexed(
        commandBuffer,
        meshes[0].submeshes[renderObjects[1].submeshIndex].indexCount, 1,
        meshes[0].submeshes[renderObjects[1].submeshIndex].firstIndex,
        meshes[0].submeshes[renderObjects[1].submeshIndex].vertexOffset, 0);

    vkCmdEndRenderPass(commandBuffer);
    THROW_IF_FAILED(
        vkEndCommandBuffer(commandBuffer),
        __FILE__, __LINE__,
        "Failed to end command buffer");
}

void Cubes::updateFrameResources(u32 frameIndex)
{
    {
        Pass pass = {};
        pass.view = camera.matrices.view;
        pass.proj = camera.matrices.proj;
        pass.light.pos = glm::vec3(glm::translate(glm::mat4(1.f), lightPos) * glm::vec4(0.f, 0.f, 0.f, 1.f));
        pass.light.ambient = ambientColor;
        pass.light.diffuse = diffuseColor;
        pass.light.specular = glm::vec3(1.f);
        pass.viewPos = camera.pos;
        memcpy(frameResources[frameIndex].passBuffer.mapped, &pass, sizeof(pass));
    }
    {
        // materials
    }
    {
        // render objects
    }
}

void Cubes::destroyPipelines()
{
    for (u32 i = 0; i < pipelineLayouts.size(); ++i) {
        vkDestroyPipelineLayout(globals.device.handle, pipelineLayouts[i], globals.allocator);
    }

    for (u32 i = 0; i < pipelines.size(); ++i) {
        vkDestroyPipeline(globals.device.handle, pipelines[i], globals.allocator);
    }
}

void Cubes::destroyPushConstantRanges()
{
}

void Cubes::destroyResourceDescriptors()
{
    for (u32 i = 0; i < resourceDescriptors.size(); ++i) {
        vkDestroyDescriptorSetLayout(globals.device.handle, resourceDescriptors[i].setLayout, globals.allocator);
    }

    for (u32 i = 0; i < resourceDescriptors.size(); ++i) {
        vkDestroyDescriptorPool(globals.device.handle, resourceDescriptors[i].pool, globals.allocator);
    }
}

void Cubes::destroyFrameResources()
{
    for (u32 i = 0; i < frameResources.size(); ++i) {
        vkUnmapMemory(globals.device.handle, frameResources[i].passBuffer.memory);
        destroyBuffer(globals, frameResources[i].passBuffer);
        destroyBuffer(globals, frameResources[i].materialBuffer);
        destroyBuffer(globals, frameResources[i].renderObjectBuffer);
    }
}

void Cubes::destroyTextures()
{
    for (u32 i = 0; i < textures.size(); ++i) {
        vkDestroySampler(globals.device.handle, textures[i].sampler.handle, globals.allocator);
        vkDestroyImageView(globals.device.handle, textures[i].image.view, globals.allocator);
        destroyImage(globals, textures[i].image);
    }
}

void Cubes::destroyMeshes()
{
    for (u32 i = 0; i < meshes.size(); ++i) {
        destroyBuffer(globals, meshes[i].indexBuffer);
        destroyBuffer(globals, meshes[i].vertexBuffer);
    }
}

std::unique_ptr<SampleBase> createSample()
{
    return std::make_unique<Cubes>(1600, 900, "Cubes");
}
