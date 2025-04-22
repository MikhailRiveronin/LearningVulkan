#include "FrameResources.h"
#include "Boilerplate/Entry.h"
#include "Boilerplate/EventManager.h"
#include "Boilerplate/ProceduralMeshes/Box.h"
#include "Boilerplate/Structures.h"
#include "Boilerplate/Utils.h"
#include "Boilerplate/SampleBase.h"
#include "Boilerplate/Application.h"
#include "Boilerplate/Initializer.h"

#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <chrono>
#include <memory>
#include <unordered_map>

class Cubes : public SampleBase {
public:
    Cubes(u32 width, u32 height, std::string const& name);

private:
    struct PassData {
        glm::mat4 view;
        glm::mat4 proj;
    };

    struct ObjectData {
        glm::mat4 world;
    };

    Camera camera;
    Buffer vertexBuffer;
    Buffer indexBuffer;

    struct {
        std::array<RenderObject, 1> cube;
        std::array<RenderObject, 1> lightCube;
    } renderObjects;

    struct {
        struct {
            Buffer mainPassBuffer;
        } passes;

        struct {
            struct {
                Buffer buffer;
                Image image;
            } cube;

            struct {
                Buffer buffer;
            } lightCube;
        } objects;
    } frameResources;




    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    struct {
        Image image;
        Sampler sampler;
    } texture;

    void createRenderObjects() override;
    void createPipelines() override;
    void createFrameResources() override;
    void createMesh() override;
    void createTexture() override;

    void createDescriptorSets() override;

    void updateFrameResources(u32 frameIndex) override;
    void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex) override;

    void destroyDescriptorSets() override;
    void destroyTextureImage() override;
    void destroyUniformBuffers() override;
    void destroyIndexBuffer() override;
    void destroyVertexBuffer() override;
    void destroyGraphicsPipeline() override;
    void destroyPipelineLayout() override;
    void destroyDescriptorSetLayouts() override;
    void destroyFrameResources() override;
    void destroyRenderObjects() override;
};

Cubes::Cubes(u32 width, u32 height, std::string const& name) :
    SampleBase(width, height, name),
    camera(globals, glm::vec3(0.f, 0.f, 3.f)),
    renderLayers(2),
    frameResources(framesInFlight)
{
    EventManager::subscribe(EventType::LEFT_BUTTON_DOWN, &camera);
    EventManager::subscribe(EventType::MOUSE_MOVE, &camera);
    EventManager::subscribe(EventType::KEY_DOWN, &camera);
    EventManager::subscribe(EventType::WINDOW_RESIZE, this);
}

void Cubes::createPipelines()
{
    // cube
    {
        // Stages
        std::vector<VkPipelineShaderStageCreateInfo> stages(2);
        VkShaderModule vertexShaderModule;
        VkShaderModule fragmentShaderModule;
        auto vertexShaderCode = loadShaderCode("Cubes/CubeVertex.spv");
        auto fragmentShaderCode = loadShaderCode("Cubes/CubeFragment.spv");
        auto vertexShaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(vertexShaderCode);
        auto fragmentShaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(fragmentShaderCode);
        THROW_IF_FAILED(
            vkCreateShaderModule(globals.device.handle, &vertexShaderModuleCreateInfo, globals.allocator, &vertexShaderModule),
            __FILE__, __LINE__,
            "Failed to create shader module");
        THROW_IF_FAILED(
            vkCreateShaderModule(globals.device.handle, &fragmentShaderModuleCreateInfo, globals.allocator, &fragmentShaderModule),
            __FILE__, __LINE__,
            "Failed to create shader module");
        stages[0] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShaderModule);
        stages[1] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShaderModule);
        vkDestroyShaderModule(globals.device.handle, vertexShaderModule, globals.allocator);
        vkDestroyShaderModule(globals.device.handle, fragmentShaderModule, globals.allocator);

        // Vertex input state
        std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions(1);
        vertexBindingDescriptions[0] = Initializer::vertexInputBindingDescription(0, sizeof(Vertex));
        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions(3);
        vertexAttributeDescriptions[0] = Initializer::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos));
        vertexAttributeDescriptions[1] = Initializer::vertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal));
        vertexAttributeDescriptions[2] = Initializer::vertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord));
        auto vertexInputState = Initializer::pipelineVertexInputStateCreateInfo(vertexBindingDescriptions, vertexAttributeDescriptions);

        // Input assembly state
        auto inputAssemblyState = Initializer::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        // Tessellation state
        auto tessellationState = Initializer::pipelineTessellationStateCreateInfo();

        // Viewport state
        std::vector<VkViewport> viewports(1);
        viewports[0] = Initializer::viewport(globals.swapchain.extent.width, globals.swapchain.extent.height);
        std::vector<VkRect2D> scissors(1);
        scissors[0] = Initializer::scissor(globals.swapchain.extent.width, globals.swapchain.extent.height);
        auto viewportState = Initializer::pipelineViewportStateCreateInfo(viewports, scissors);

        // Rasterization state
        auto rasterizationState = Initializer::pipelineRasterizationStateCreateInfo();

        // Multisample state
        auto multisampleState = Initializer::pipelineMultisampleStateCreateInfo();

        // Depth stencil state
        auto depthStencilState = Initializer::pipelineDepthStencilStateCreateInfo();

        // Color blend state
        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates(1);
        colorBlendAttachmentStates[0] = Initializer::pipelineColorBlendAttachmentState();
        auto colorBlendState = Initializer::pipelineColorBlendStateCreateInfo(colorBlendAttachmentStates);

        // Dynamic state
        std::vector<VkDynamicState> dynamicStates(2);
        dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
        auto dynamicState = Initializer::pipelineDynamicStateCreateInfo(dynamicStates);

        // Layout
        std::vector<VkDescriptorSetLayoutBinding> bindings(3);
        bindings[0] = Initializer::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
        bindings[1] = Initializer::descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
        bindings[2] = Initializer::descriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        renderLayers[0].setLayouts.resize(1);
        auto descriptorSetLayoutCreateInfo = Initializer::descriptorSetLayoutCreateInfo(bindings);
        THROW_IF_FAILED(
            vkCreateDescriptorSetLayout(globals.device.handle, &descriptorSetLayoutCreateInfo, globals.allocator, &renderLayers[0].setLayouts[0]),
            __FILE__, __LINE__,
            "Failed to create descriptor set layout");
        VkPipelineLayout layout;
        std::vector<VkPushConstantRange> pushConstantRanges;
        auto pipelineLayoutCreateInfo = Initializer::pipelineLayoutCreateInfo(renderLayers[0].setLayouts, pushConstantRanges);
        THROW_IF_FAILED(
            vkCreatePipelineLayout(globals.device.handle, &pipelineLayoutCreateInfo, globals.allocator, &layout),
            __FILE__, __LINE__,
            "Failed to create pipeline layout");

        // Pipeline
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
            layout,
            globals.renderPass);
        THROW_IF_FAILED(
            vkCreateGraphicsPipelines(globals.device.handle, VK_NULL_HANDLE, 1, &createInfo, globals.allocator, &renderLayers[0].pipeline),
            __FILE__, __LINE__,
            "Failed to create graphics pipeline");
    }

    // lightCube
    {
        // Stages
        std::vector<VkPipelineShaderStageCreateInfo> stages(2);
        VkShaderModule vertexShaderModule;
        VkShaderModule fragmentShaderModule;
        auto vertexShaderCode = loadShaderCode("Cubes/LightCubeVertex.spv");
        auto fragmentShaderCode = loadShaderCode("Cubes/LightCubeFragment.spv");
        auto vertexShaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(vertexShaderCode);
        auto fragmentShaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(fragmentShaderCode);
        THROW_IF_FAILED(
            vkCreateShaderModule(globals.device.handle, &vertexShaderModuleCreateInfo, globals.allocator, &vertexShaderModule),
            __FILE__, __LINE__,
            "Failed to create shader module");
        THROW_IF_FAILED(
            vkCreateShaderModule(globals.device.handle, &fragmentShaderModuleCreateInfo, globals.allocator, &fragmentShaderModule),
            __FILE__, __LINE__,
            "Failed to create shader module");
        stages[0] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShaderModule);
        stages[1] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShaderModule);
        vkDestroyShaderModule(globals.device.handle, vertexShaderModule, globals.allocator);
        vkDestroyShaderModule(globals.device.handle, fragmentShaderModule, globals.allocator);

        // Vertex input state
        std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions(1);
        vertexBindingDescriptions[0] = Initializer::vertexInputBindingDescription(0, sizeof(Vertex));
        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions(3);
        vertexAttributeDescriptions[0] = Initializer::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos));
        vertexAttributeDescriptions[1] = Initializer::vertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal));
        vertexAttributeDescriptions[2] = Initializer::vertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord));
        auto vertexInputState = Initializer::pipelineVertexInputStateCreateInfo(vertexBindingDescriptions, vertexAttributeDescriptions);

        // Input assembly state
        auto inputAssemblyState = Initializer::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        // Tessellation state
        auto tessellationState = Initializer::pipelineTessellationStateCreateInfo();

        // Viewport state
        std::vector<VkViewport> viewports(1);
        viewports[0] = Initializer::viewport(globals.swapchain.extent.width, globals.swapchain.extent.height);
        std::vector<VkRect2D> scissors(1);
        scissors[0] = Initializer::scissor(globals.swapchain.extent.width, globals.swapchain.extent.height);
        auto viewportState = Initializer::pipelineViewportStateCreateInfo(viewports, scissors);

        // Rasterization state
        auto rasterizationState = Initializer::pipelineRasterizationStateCreateInfo();

        // Multisample state
        auto multisampleState = Initializer::pipelineMultisampleStateCreateInfo();

        // Depth stencil state
        auto depthStencilState = Initializer::pipelineDepthStencilStateCreateInfo();

        // Color blend state
        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates(1);
        colorBlendAttachmentStates[0] = Initializer::pipelineColorBlendAttachmentState();
        auto colorBlendState = Initializer::pipelineColorBlendStateCreateInfo(colorBlendAttachmentStates);

        // Dynamic state
        std::vector<VkDynamicState> dynamicStates(2);
        dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
        auto dynamicState = Initializer::pipelineDynamicStateCreateInfo(dynamicStates);

        // Layout
        std::vector<VkDescriptorSetLayoutBinding> bindings(2);
        bindings[0] = Initializer::descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
        bindings[1] = Initializer::descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
        renderLayers[1].setLayouts.resize(1);
        auto descriptorSetLayoutCreateInfo = Initializer::descriptorSetLayoutCreateInfo(bindings);
        THROW_IF_FAILED(
            vkCreateDescriptorSetLayout(globals.device.handle, &descriptorSetLayoutCreateInfo, globals.allocator, &renderLayers[1].setLayouts[0]),
            __FILE__, __LINE__,
            "Failed to create descriptor set layout");
        VkPipelineLayout layout;
        std::vector<VkPushConstantRange> pushConstantRanges;
        auto pipelineLayoutCreateInfo = Initializer::pipelineLayoutCreateInfo(renderLayers[1].setLayouts, pushConstantRanges);
        THROW_IF_FAILED(
            vkCreatePipelineLayout(globals.device.handle, &pipelineLayoutCreateInfo, globals.allocator, &layout),
            __FILE__, __LINE__,
            "Failed to create pipeline layout");

        // Pipeline
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
            layout,
            globals.renderPass);
        THROW_IF_FAILED(
            vkCreateGraphicsPipelines(globals.device.handle, VK_NULL_HANDLE, 1, &createInfo, globals.allocator, &renderLayers[1].pipeline),
            __FILE__, __LINE__,
            "Failed to create graphics pipeline");
    }
}

void Cubes::createRenderObjects()
{
    renderObjects.cube[0].mesh.vertices = Box::vertices;
    renderObjects.cube[0].mesh.indices = Box::indices;
    renderObjects.cube[0].drawArgs.indexCount = renderObjects.cube[0].mesh.indices.size();
    renderObjects.cube[0].drawArgs.firstIndex = 0;
    renderObjects.cube[0].drawArgs.vertexOffset = 0;
    renderObjects.cube[0].world = glm::mat4(1.f);

    renderObjects.lightCube[1].mesh.vertices = Box::vertices;
    renderObjects.lightCube[1].mesh.indices = Box::indices;
    renderObjects.lightCube[1].drawArgs.indexCount = renderObjects.lightCube[1].mesh.indices.size();
    renderObjects.lightCube[1].drawArgs.firstIndex = renderObjects.cube[0].drawArgs.indexCount;
    renderObjects.lightCube[1].drawArgs.vertexOffset = renderObjects.cube[0].mesh.vertices.size();
    renderObjects.lightCube[1].world = glm::translate(renderObjects.lightCube[1].world, glm::vec3(1.2f, -1.f, 2.f));
}

void Cubes::createFrameResources()
{
    {
        frameResources.passes.mainPassBuffer.size = framesInFlight * sizeof(PassData);
        frameResources.passes.mainPassBuffer.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        frameResources.passes.mainPassBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        createBuffer(globals, frameResources.passes.mainPassBuffer);
        mapBuffer(globals, frameResources.passes.mainPassBuffer);
    }

    frameResources.objects.cube.buffer.size = framesInFlight * renderObjects.cube.size() * sizeof(ObjectData);




    u32 renderObjectCount = 0;
    for (u32 i = 0; i < renderLayers.size(); ++i) {
        renderObjectCount += renderLayers[i].renderObjects.size();
    }
    for (u32 i = 0; i < framesInFlight; ++i) {
        // Object buffer
        Buffer objectStagingBuffer;
        objectStagingBuffer.size = renderObjectCount * sizeof(ObjectResources);
        objectStagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        objectStagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        createBuffer(globals, objectStagingBuffer);
        mapBuffer(globals, objectStagingBuffer);
        for (u32 j = 0; j < renderLayers.size(); ++j) {
            for (u32 k = 0; k < renderLayers[j].renderObjects.size(); ++k) {
                ObjectResources objectResources;
                objectResources.world = renderLayers[j].renderObjects[k].world;
                memcpy((char*)objectStagingBuffer.mapped + (j + k) * sizeof(objectResources), &objectResources, sizeof(objectResources));
            }
        }
        vkUnmapMemory(globals.device.handle, objectStagingBuffer.memory);
        Buffer objectBuffer;
        objectBuffer.size = objectStagingBuffer.size;
        objectBuffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        objectBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        createBuffer(globals, objectBuffer);
        copyBuffer(globals, objectStagingBuffer, objectBuffer);
        destroyBuffer(globals, objectStagingBuffer);

        // Pass buffer
        frameResources[i].passBuffer.size = sizeof(PassResources);
        frameResources[i].passBuffer.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        frameResources[i].passBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        createBuffer(globals, frameResources[i].passBuffer);
        THROW_IF_FAILED(
            vkMapMemory(globals.device.handle, frameResources[i].passBuffer.memory, 0, frameResources[i].passBuffer.size, 0, &frameResources[i].passBuffer.mapped),
            __FILE__, __LINE__,
            "Failed to map memory");
    }
}

void Cubes::createMesh()
{
    std::vector<Vertex> vertices;
    std::vector<u16> indices;
    for (u32 i = 0; i < renderLayers.size(); ++i) {
        for (u32 j = 0; j < renderLayers[i].renderObjects.size(); ++j) {
            vertices.insert(
                vertices.end(),
                renderLayers[i].renderObjects[j].mesh.vertices.begin(),
                renderLayers[i].renderObjects[j].mesh.vertices.end());
            indices.insert(
                indices.end(),
                renderLayers[i].renderObjects[j].mesh.indices.begin(),
                renderLayers[i].renderObjects[j].mesh.indices.end());
        }
    }

    Buffer vertexStagingBuffer;
    vertexStagingBuffer.size = vertices.size() * sizeof(vertices[0]);
    vertexStagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    vertexStagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    createBuffer(globals, vertexStagingBuffer);
    THROW_IF_FAILED(
        vkMapMemory(globals.device.handle, vertexStagingBuffer.memory, 0, vertexStagingBuffer.size, 0, &vertexStagingBuffer.mapped),
        __FILE__, __LINE__,
        "Failed to map memory");
    memcpy(vertexStagingBuffer.mapped, vertices.data(), vertexStagingBuffer.size);
    vkUnmapMemory(globals.device.handle, vertexStagingBuffer.memory);
    vertexBuffer.size = vertexStagingBuffer.size;
    vertexBuffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    createBuffer(globals, vertexBuffer);
    copyBuffer(globals, vertexStagingBuffer, vertexBuffer);
    destroyBuffer(globals, vertexStagingBuffer);

    Buffer indexStagingBuffer;
    indexStagingBuffer.size = indices.size() * sizeof(indices[0]);
    indexStagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    indexStagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    createBuffer(globals, indexStagingBuffer);
    THROW_IF_FAILED(
        vkMapMemory(globals.device.handle, indexStagingBuffer.memory, 0, indexStagingBuffer.size, 0, &indexStagingBuffer.mapped),
        __FILE__, __LINE__,
        "Failed to map memory");
    memcpy(indexStagingBuffer.mapped, indices.data(), indexStagingBuffer.size);
    vkUnmapMemory(globals.device.handle, indexStagingBuffer.memory);
    indexBuffer.size = indexStagingBuffer.size;
    indexBuffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    indexBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    createBuffer(globals, indexBuffer);
    copyBuffer(globals, indexStagingBuffer, indexBuffer);
    destroyBuffer(globals, indexStagingBuffer);
}

void Cubes::createTexture()
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

    texture.image.width = width;
    texture.image.height = height;
    texture.image.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    texture.image.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    createImage(globals, texture.image);
    transitionImageLayout(globals, texture.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(globals, stagingBuffer, texture.image);
    destroyBuffer(globals, stagingBuffer);
    transitionImageLayout(globals, texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    createImageView(globals, texture.image);
    createSampler(globals, texture.sampler);
}

void Cubes::createDescriptorSets()
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    // Object resources
    {
        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = framesInFlight * renderObjectCount;
        poolSizes.push_back(poolSize);
    }
    // Pass resources
    {
        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = framesInFlight;
        poolSizes.push_back(poolSize);
    }
    // Texture
    {
        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = framesInFlight * renderLayers[0].renderObjects.size();
        poolSizes.push_back(poolSize);
    }

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolCreateInfo.maxSets = framesInFlight * renderObjectCount;
    descriptorPoolCreateInfo.poolSizeCount = poolSizes.size();
    descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
    THROW_IF_FAILED(
        vkCreateDescriptorPool(globals.device.handle, &descriptorPoolCreateInfo, globals.allocator, &descriptorPool),
        __FILE__, __LINE__,
        "Failed to create descriptor pool");

    // cube
    {
        renderLayers[0].descriptorSets.resize(framesInFlight * renderLayers[0].renderObjects.size());
        std::vector<VkDescriptorSetLayout> setLayouts(framesInFlight * renderLayers[0].renderObjects.size(), renderLayers[0].setLayouts[0]);
        VkDescriptorSetAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.pNext = nullptr;
        allocateInfo.descriptorPool = descriptorPool;
        allocateInfo.descriptorSetCount = framesInFlight * renderLayers[0].renderObjects.size();
        allocateInfo.pSetLayouts = setLayouts.data();
        THROW_IF_FAILED(
            vkAllocateDescriptorSets(globals.device.handle, &allocateInfo, renderLayers[0].descriptorSets.data()),
            __FILE__, __LINE__,
            "Failed to allocate descriptor sets");
        LOG_DEBUG("Descriptor sets successfully created");

        for (u32 i = 0; i < framesInFlight; ++i) {
            for (u32 j = 0; j < renderLayers[0].descriptorSets.size(); ++j) {
                std::vector<VkWriteDescriptorSet> descriptorWrites;

                VkDescriptorBufferInfo objectBufferInfo = {};
                objectBufferInfo.buffer = frameResources[i].objectBuffer.handle;
                objectBufferInfo.offset = j * sizeof(ObjectResources);
                objectBufferInfo.range = sizeof(ObjectResources);

                VkDescriptorBufferInfo passBufferInfo = {};
                passBufferInfo.buffer = frameResources[i].passBuffer.handle;
                passBufferInfo.offset = j * sizeof(PassResources);
                passBufferInfo.range = sizeof(PassResources);

                VkDescriptorImageInfo imageInfo = {};
                imageInfo.sampler = texture.sampler.handle;
                imageInfo.imageView = texture.image.view;
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                VkWriteDescriptorSet objectDescriptorWrite = {};
                objectDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                objectDescriptorWrite.pNext = nullptr;
                objectDescriptorWrite.dstSet = renderLayers[0].descriptorSets[j];
                objectDescriptorWrite.dstBinding = 0;
                objectDescriptorWrite.dstArrayElement = 0;
                objectDescriptorWrite.descriptorCount = 1;
                objectDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                objectDescriptorWrite.pImageInfo = nullptr;
                objectDescriptorWrite.pBufferInfo = &objectBufferInfo;
                objectDescriptorWrite.pTexelBufferView = nullptr;
                descriptorWrites.push_back(objectDescriptorWrite);

                VkWriteDescriptorSet passDescriptorWrite = {};
                passDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                passDescriptorWrite.pNext = nullptr;
                passDescriptorWrite.dstSet = renderLayers[0].descriptorSets[j];
                passDescriptorWrite.dstBinding = 1;
                passDescriptorWrite.dstArrayElement = 0;
                passDescriptorWrite.descriptorCount = 1;
                passDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                passDescriptorWrite.pImageInfo = nullptr;
                passDescriptorWrite.pBufferInfo = &passBufferInfo;
                passDescriptorWrite.pTexelBufferView = nullptr;
                descriptorWrites.push_back(passDescriptorWrite);

                VkWriteDescriptorSet samplerDescriptorWrite = {};
                samplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                samplerDescriptorWrite.pNext = nullptr;
                samplerDescriptorWrite.dstSet = renderLayers[0].descriptorSets[j];
                samplerDescriptorWrite.dstBinding = 2;
                samplerDescriptorWrite.dstArrayElement = 0;
                samplerDescriptorWrite.descriptorCount = 1;
                samplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                samplerDescriptorWrite.pImageInfo = &imageInfo;
                samplerDescriptorWrite.pBufferInfo = nullptr;
                samplerDescriptorWrite.pTexelBufferView = nullptr;
                descriptorWrites.push_back(samplerDescriptorWrite);

                vkUpdateDescriptorSets(globals.device.handle, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
            }
        }
    }

    // lightCube
    {
        renderLayers[1].descriptorSets.resize(framesInFlight * renderLayers[1].renderObjects.size());
        std::vector<VkDescriptorSetLayout> setLayouts(framesInFlight * renderLayers[1].renderObjects.size(), renderLayers[1].setLayouts[0]);
        VkDescriptorSetAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.pNext = nullptr;
        allocateInfo.descriptorPool = descriptorPool;
        allocateInfo.descriptorSetCount = framesInFlight * renderLayers[1].renderObjects.size();
        allocateInfo.pSetLayouts = setLayouts.data();
        THROW_IF_FAILED(
            vkAllocateDescriptorSets(globals.device.handle, &allocateInfo, renderLayers[1].descriptorSets.data()),
            __FILE__, __LINE__,
            "Failed to allocate descriptor sets");
        LOG_DEBUG("Descriptor sets successfully created");

        for (u32 i = 0; i < framesInFlight; ++i) {
            for (u32 j = 0; j < renderLayers[1].descriptorSets.size(); ++j) {
                std::vector<VkWriteDescriptorSet> descriptorWrites;

                VkDescriptorBufferInfo objectBufferInfo = {};
                objectBufferInfo.buffer = frameResources[i].objectBuffer.handle;
                objectBufferInfo.offset = j * sizeof(ObjectResources);
                objectBufferInfo.range = sizeof(ObjectResources);

                VkDescriptorBufferInfo passBufferInfo = {};
                passBufferInfo.buffer = frameResources[i].passBuffer.handle;
                passBufferInfo.offset = j * sizeof(PassResources);
                passBufferInfo.range = sizeof(PassResources);

                VkDescriptorImageInfo imageInfo = {};
                imageInfo.sampler = texture.sampler.handle;
                imageInfo.imageView = texture.image.view;
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                VkWriteDescriptorSet objectDescriptorWrite = {};
                objectDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                objectDescriptorWrite.pNext = nullptr;
                objectDescriptorWrite.dstSet = renderLayers[0].descriptorSets[j];
                objectDescriptorWrite.dstBinding = 0;
                objectDescriptorWrite.dstArrayElement = 0;
                objectDescriptorWrite.descriptorCount = 1;
                objectDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                objectDescriptorWrite.pImageInfo = nullptr;
                objectDescriptorWrite.pBufferInfo = &objectBufferInfo;
                objectDescriptorWrite.pTexelBufferView = nullptr;
                descriptorWrites.push_back(objectDescriptorWrite);

                VkWriteDescriptorSet passDescriptorWrite = {};
                passDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                passDescriptorWrite.pNext = nullptr;
                passDescriptorWrite.dstSet = renderLayers[0].descriptorSets[j];
                passDescriptorWrite.dstBinding = 1;
                passDescriptorWrite.dstArrayElement = 0;
                passDescriptorWrite.descriptorCount = 1;
                passDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                passDescriptorWrite.pImageInfo = nullptr;
                passDescriptorWrite.pBufferInfo = &passBufferInfo;
                passDescriptorWrite.pTexelBufferView = nullptr;
                descriptorWrites.push_back(passDescriptorWrite);

                VkWriteDescriptorSet samplerDescriptorWrite = {};
                samplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                samplerDescriptorWrite.pNext = nullptr;
                samplerDescriptorWrite.dstSet = renderLayers[0].descriptorSets[j];
                samplerDescriptorWrite.dstBinding = 2;
                samplerDescriptorWrite.dstArrayElement = 0;
                samplerDescriptorWrite.descriptorCount = 1;
                samplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                samplerDescriptorWrite.pImageInfo = &imageInfo;
                samplerDescriptorWrite.pBufferInfo = nullptr;
                samplerDescriptorWrite.pTexelBufferView = nullptr;
                descriptorWrites.push_back(samplerDescriptorWrite);

                vkUpdateDescriptorSets(globals.device.handle, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
            }
        }
    }
}


void Cubes::recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex)
{
    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;
    THROW_IF_FAILED(
        vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo),
        __FILE__, __LINE__,
        "Failed to begin command buffer");

    VkClearValue clearColor = {};
    clearColor.color = {{ 0.f, 0.f, 0.f, 1.f }};

    VkClearValue clearDepth = {};
    clearDepth.depthStencil = { 1.f, 0 };

    std::vector<VkClearValue> clearValues;
    clearValues.push_back(std::move(clearColor));
    clearValues.push_back(std::move(clearDepth));

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = globals.renderPass;
    renderPassBeginInfo.framebuffer = globals.swapchain.framebuffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    renderPassBeginInfo.renderArea.extent = globals.swapchain.extent;
    renderPassBeginInfo.clearValueCount = clearValues.size();
    renderPassBeginInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, globals.graphicsPipeline.pipeline);

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y =0;
    viewport.width = globals.swapchain.extent.width;
    viewport.height = globals.swapchain.extent.height;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = { globals.swapchain.extent.width, globals.swapchain.extent.height };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.handle, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer.handle, 0, VK_INDEX_TYPE_UINT16);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, globals.graphicsPipeline.layout, 0, 1, &descriptorSets[frameIndex], 0, nullptr);
    vkCmdDrawIndexed(commandBuffer, indices.size(), 1, 0, 0, 0);
    vkCmdEndRenderPass(commandBuffer);

    THROW_IF_FAILED(
        vkEndCommandBuffer(commandBuffer),
        __FILE__, __LINE__,
        "Failed to end command buffer");
}

void Cubes::updateFrameResources(u32 frameIndex)
{
    PassResources passResources = {};
    passResources.view = camera.matrices.view;
    passResources.proj = camera.matrices.proj;
    memcpy(frameResources[frameIndex].passBuffer.mapped, &passResources, sizeof(passResources));
}

void Cubes::destroyDescriptorSets()
{
    for (u32 i = 0; i < framesInFlight; ++i) {
        vkFreeDescriptorSets(globals.device.handle, descriptorPool, 1, &descriptorSets[i]);
    }
    vkDestroyDescriptorPool(globals.device.handle, descriptorPool, globals.allocator);
    LOG_DEBUG("Descriptor sets destroyed");
}

void Cubes::destroyTextureImage()
{
    vkDestroySampler(globals.device.handle, sampler.handle, globals.allocator);
    vkDestroyImageView(globals.device.handle, image.view, globals.allocator);
    destroyImage(globals, image);
    LOG_DEBUG("Texture image destroyed");
}

void Cubes::destroyUniformBuffers()
{
    for (u32 i = 0; i < framesInFlight; ++i) {
        vkUnmapMemory(globals.device.handle, uniformBuffers[i].memory);
        destroyBuffer(globals, uniformBuffers[i]);
    }
    LOG_DEBUG("Uniform buffers destroyed");
}

void Cubes::destroyIndexBuffer()
{
    destroyBuffer(globals, indexBuffer);
    LOG_DEBUG("Index buffer destroyed");
}

void Cubes::destroyVertexBuffer()
{
    destroyBuffer(globals, vertexBuffer);
    LOG_DEBUG("Vertex buffer destroyed");
}

void Cubes::destroyGraphicsPipeline()
{
    vkDestroyPipeline(globals.device.handle, globals.graphicsPipeline.pipeline, globals.allocator);
    LOG_DEBUG("Pipeline destroyed");
}

void Cubes::destroyPipelineLayout()
{
    vkDestroyPipelineLayout(globals.device.handle, globals.graphicsPipeline.layout, globals.allocator);
    LOG_DEBUG("Pipeline layout destroyed");
}

void Cubes::destroyFrameResources()
{
    for (u32 i = 0; i < framesInFlight; ++i) {
        vkUnmapMemory(globals.device.handle, frameResources[i].objectBuffer.memory);
        destroyBuffer(globals, frameResources[i].objectBuffer);

        vkUnmapMemory(globals.device.handle, frameResources[i].passResources.memory);
        destroyBuffer(globals, frameResources[i].passResources);
    }
}

void Cubes::destroyRenderObjects()
{
}

std::unique_ptr<SampleBase> createSample()
{
    return std::make_unique<Cubes>(1600, 900, "Cubes");
}
