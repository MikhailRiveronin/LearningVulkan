#include "Mesh.h"
#include "Boilerplate/Entry.h"
#include "Boilerplate/EventManager.h"
#include "Boilerplate/Structures.h"
#include "Boilerplate/Utils.h"
#include "Boilerplate/SampleBase.h"
#include "Boilerplate/Application.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <chrono>
#include <memory>

class Cubes : public SampleBase {
public:
    Cubes(u32 width, u32 height, std::string const& name);

private:
    struct UniformBufferObject {
        glm::mat4 world;
        glm::mat4 view;
        glm::mat4 proj;
    };

    Buffer vertexBuffer;
    Buffer indexBuffer;
    std::vector<Buffer> uniformBuffers;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    Image texture;
    Sampler sampler;

    Camera camera;

    void createDescriptorSetLayout() override;
    void createPipelineLayout() override;
    void createGraphicsPipeline() override;
    void createVertexBuffer() override;
    void createIndexBuffer() override;
    void createUniformBuffers() override;
    void createTextureImage() override;
    void createDescriptorSets() override;

    void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex) override;
    void updateUniformBuffer(u32 frameIndex) override;

    void destroyDescriptorSets() override;
    void destroyTextureImage() override;
    void destroyUniformBuffers() override;
    void destroyIndexBuffer() override;
    void destroyVertexBuffer() override;
    void destroyGraphicsPipeline() override;
    void destroyPipelineLayout() override;
    void destroyDescriptorSetLayout() override;
};

Cubes::Cubes(u32 width, u32 height, std::string const& name) :
    SampleBase(width, height, name),
    camera(globals, glm::vec3(0.f, 0.f, 3.f))
{
    EventManager::subscribe(EventType::LEFT_BUTTON_DOWN, &camera);
    EventManager::subscribe(EventType::MOUSE_MOVE, &camera);
    EventManager::subscribe(EventType::KEY_DOWN, &camera);
}

void Cubes::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    samplerLayoutBinding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    layoutBindings.push_back(std::move(uboLayoutBinding));
    layoutBindings.push_back(std::move(samplerLayoutBinding));

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.bindingCount = layoutBindings.size();
    createInfo.pBindings = layoutBindings.data();
    THROW_IF_FAILED(
        vkCreateDescriptorSetLayout(globals.device.handle, &createInfo, globals.allocator, &globals.graphicsPipeline.descriptorSetLayout),
        __FILE__, __LINE__,
        "Failed to create descriptor set layout");
    LOG_DEBUG("Descriptor set layout successfully created");
}

void Cubes::createPipelineLayout()
{
    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.setLayoutCount = 1;
    createInfo.pSetLayouts = &globals.graphicsPipeline.descriptorSetLayout;
    createInfo.pushConstantRangeCount = 0;
    createInfo.pPushConstantRanges = nullptr;
    THROW_IF_FAILED(
        vkCreatePipelineLayout(globals.device.handle, &createInfo, globals.allocator, &globals.graphicsPipeline.layout),
        __FILE__, __LINE__,
        "Failed to create pipeline layout");
    LOG_DEBUG("Pipeline layout successfully created");
}

void Cubes::createGraphicsPipeline()
{
    auto vertexShaderCode = loadShaderBinary("Shaders/Vertex.spv");
    auto fragmentShaderCode = loadShaderBinary("Shaders/Fragment.spv");
    auto vertexShaderModule = createShaderModule(globals, vertexShaderCode);
    auto fragmentShaderModule = createShaderModule(globals, fragmentShaderCode);

    VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = {};
    vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageCreateInfo.pNext = nullptr;
    vertexShaderStageCreateInfo.flags = 0;
    vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageCreateInfo.module = vertexShaderModule;
    vertexShaderStageCreateInfo.pName = "main";
    vertexShaderStageCreateInfo.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = {};
    fragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageCreateInfo.pNext = nullptr;
    fragmentShaderStageCreateInfo.flags = 0;
    fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageCreateInfo.module = fragmentShaderModule;
    fragmentShaderStageCreateInfo.pName = "main";
    fragmentShaderStageCreateInfo.pSpecializationInfo = nullptr;

    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
    shaderStageCreateInfos.push_back(std::move(vertexShaderStageCreateInfo));
    shaderStageCreateInfos.push_back(std::move(fragmentShaderStageCreateInfo));

    VkVertexInputBindingDescription vertexInputBindingDescription = {};
    vertexInputBindingDescription.binding = 0;
    vertexInputBindingDescription.stride = sizeof(Vertex);
    vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription positionVertexInputAttributeDescription = {};
    positionVertexInputAttributeDescription.location = 0;
    positionVertexInputAttributeDescription.binding = 0;
    positionVertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    positionVertexInputAttributeDescription.offset = offsetof(Vertex, pos);

    VkVertexInputAttributeDescription texCoordsVertexInputAttributeDescription = {};
    texCoordsVertexInputAttributeDescription.location = 1;
    texCoordsVertexInputAttributeDescription.binding = 0;
    texCoordsVertexInputAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
    texCoordsVertexInputAttributeDescription.offset = offsetof(Vertex, texCoords);

    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
    vertexInputAttributeDescriptions.push_back(std::move(positionVertexInputAttributeDescription));
    vertexInputAttributeDescriptions.push_back(std::move(texCoordsVertexInputAttributeDescription));

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.pNext = nullptr;
    vertexInputStateCreateInfo.flags = 0;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexInputAttributeDescriptions.size();
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.pNext = nullptr;
    inputAssemblyStateCreateInfo.flags = 0;
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = globals.swapchain.extent.width;
    viewport.height = globals.swapchain.extent.height;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = globals.swapchain.extent;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.pNext = nullptr;
    viewportStateCreateInfo.flags = 0;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.pNext = nullptr;
    rasterizationStateCreateInfo.flags = 0;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.f;
    rasterizationStateCreateInfo.depthBiasClamp = 0.f;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.f;
    rasterizationStateCreateInfo.lineWidth = 1.f;

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.pNext = nullptr;
    depthStencilStateCreateInfo.flags = 0;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.front = {};
    depthStencilStateCreateInfo.back = {};
    depthStencilStateCreateInfo.minDepthBounds = 0.f;
    depthStencilStateCreateInfo.maxDepthBounds = 1.f;

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.pNext = nullptr;
    multisampleStateCreateInfo.flags = 0;
    multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.minSampleShading = 0.f;
    multisampleStateCreateInfo.pSampleMask = nullptr;
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.blendEnable = VK_FALSE;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.pNext = nullptr;
    colorBlendStateCreateInfo.flags = 0;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_CLEAR;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
    colorBlendStateCreateInfo.blendConstants[0] = { 0.f };
    colorBlendStateCreateInfo.blendConstants[1] = { 0.f };
    colorBlendStateCreateInfo.blendConstants[2] = { 0.f };
    colorBlendStateCreateInfo.blendConstants[3] = { 0.f };

    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT };
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.pNext = nullptr;
    dynamicStateCreateInfo.flags = 0;
    dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size();
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.stageCount = shaderStageCreateInfos.size();
    createInfo.pStages = shaderStageCreateInfos.data();
    createInfo.pVertexInputState = &vertexInputStateCreateInfo;
    createInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    createInfo.pTessellationState = nullptr;
    createInfo.pViewportState = &viewportStateCreateInfo;
    createInfo.pRasterizationState = &rasterizationStateCreateInfo;
    createInfo.pMultisampleState = &multisampleStateCreateInfo;
    createInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    createInfo.pColorBlendState = &colorBlendStateCreateInfo;
    createInfo.pDynamicState = &dynamicStateCreateInfo;
    createInfo.layout = globals.graphicsPipeline.layout;
    createInfo.renderPass = globals.renderPass;
    createInfo.subpass = 0;
    createInfo.basePipelineHandle = VK_NULL_HANDLE;
    createInfo.basePipelineIndex = 0;
    THROW_IF_FAILED(
        vkCreateGraphicsPipelines(globals.device.handle, VK_NULL_HANDLE, 1, &createInfo, globals.allocator, &globals.graphicsPipeline.pipeline),
        __FILE__, __LINE__,
        "Failed to create graphics pipeline");
    LOG_DEBUG("Graphics pipeline successfully created");

    vkDestroyShaderModule(globals.device.handle, vertexShaderModule, globals.allocator);
    vkDestroyShaderModule(globals.device.handle, fragmentShaderModule, globals.allocator);
}

void Cubes::createVertexBuffer()
{
    for (u32 i = 0; i < 10; ++i) {
        for (u32 j = 0; j < cubeVertices.size(); ++j) {
            glm::mat4 world = glm::translate(glm::mat4(1.f), positions[i]);
            float angle = 20.f * i;
            world = glm::rotate(world, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            glm::vec3 pos = cubeVertices[j].pos;
            pos = glm::vec3(world * glm::vec4(pos, 1.f));
            vertices.push_back(Vertex{ pos, cubeVertices[j].texCoords });
        }
    }

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

    vertexBuffer.size = stagingBuffer.size;
    vertexBuffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    createBuffer(globals, vertexBuffer);

    copyBuffer(globals, stagingBuffer, vertexBuffer);
    destroyBuffer(globals, stagingBuffer);
    LOG_DEBUG("Vertex buffer successfully created");
}

void Cubes::createIndexBuffer()
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

    indexBuffer.size = stagingBuffer.size;
    indexBuffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    indexBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    createBuffer(globals, indexBuffer);

    copyBuffer(globals, stagingBuffer, indexBuffer);
    destroyBuffer(globals, stagingBuffer);
    LOG_DEBUG("Index buffer successfully created");
}

void Cubes::createUniformBuffers()
{
    uniformBuffers.resize(framesInFlight);
    for (auto& uniformBuffer : uniformBuffers) {
        uniformBuffer.size = sizeof(UniformBufferObject);
        uniformBuffer.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        uniformBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        createBuffer(globals, uniformBuffer);

        THROW_IF_FAILED(
            vkMapMemory(globals.device.handle, uniformBuffer.memory, 0, uniformBuffer.size, 0, &uniformBuffer.mapped),
            __FILE__, __LINE__,
            "Failed to map memory");
    }
    LOG_DEBUG("Uniform buffers successfully created");
}

void Cubes::createTextureImage()
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

    texture.width = width;
    texture.height = height;
    texture.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    texture.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    createImage(globals, texture);

    transitionImageLayout(globals, texture, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(globals, stagingBuffer, texture);
    destroyBuffer(globals, stagingBuffer);
    transitionImageLayout(globals, texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    createImageView(globals, texture);
    createSampler(globals, sampler);
    LOG_DEBUG("Texture image successfully created");
}

void Cubes::createDescriptorSets()
{
    VkDescriptorPoolSize uboPoolSize = {};
    uboPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboPoolSize.descriptorCount = framesInFlight;

    VkDescriptorPoolSize samplerPoolSize = {};
    samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerPoolSize.descriptorCount = framesInFlight;

    std::vector<VkDescriptorPoolSize> poolSizes;
    poolSizes.push_back(std::move(uboPoolSize));
    poolSizes.push_back(std::move(samplerPoolSize));

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolCreateInfo.maxSets = framesInFlight;
    descriptorPoolCreateInfo.poolSizeCount = poolSizes.size();
    descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
    THROW_IF_FAILED(
        vkCreateDescriptorPool(globals.device.handle, &descriptorPoolCreateInfo, globals.allocator, &descriptorPool),
        __FILE__, __LINE__,
        "Failed to create descriptor pool");

    descriptorSets.resize(framesInFlight);
    std::vector<VkDescriptorSetLayout> setLayouts(framesInFlight, globals.graphicsPipeline.descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.descriptorPool = descriptorPool;
    allocateInfo.descriptorSetCount = framesInFlight;
    allocateInfo.pSetLayouts = setLayouts.data();
    THROW_IF_FAILED(
        vkAllocateDescriptorSets(globals.device.handle, &allocateInfo, descriptorSets.data()),
        __FILE__, __LINE__,
        "Failed to allocate descriptor sets");
    LOG_DEBUG("Descriptor sets successfully created");

    for (u32 i = 0; i < framesInFlight; ++i) {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = uniformBuffers[i].handle;
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;

        VkDescriptorImageInfo imageInfo = {};
        imageInfo.sampler = sampler.handle;
        imageInfo.imageView = texture.view;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet uboDescriptorWrite = {};
        uboDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uboDescriptorWrite.pNext = nullptr;
        uboDescriptorWrite.dstSet = descriptorSets[i];
        uboDescriptorWrite.dstBinding = 0;
        uboDescriptorWrite.dstArrayElement = 0;
        uboDescriptorWrite.descriptorCount = 1;
        uboDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboDescriptorWrite.pImageInfo = nullptr;
        uboDescriptorWrite.pBufferInfo = &bufferInfo;
        uboDescriptorWrite.pTexelBufferView = nullptr;

        VkWriteDescriptorSet samplerDescriptorWrite = {};
        samplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        samplerDescriptorWrite.pNext = nullptr;
        samplerDescriptorWrite.dstSet = descriptorSets[i];
        samplerDescriptorWrite.dstBinding = 1;
        samplerDescriptorWrite.dstArrayElement = 0;
        samplerDescriptorWrite.descriptorCount = 1;
        samplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerDescriptorWrite.pImageInfo = &imageInfo;
        samplerDescriptorWrite.pBufferInfo = nullptr;
        samplerDescriptorWrite.pTexelBufferView = nullptr;

        std::vector<VkWriteDescriptorSet> descriptorWrites;
        descriptorWrites.push_back(std::move(uboDescriptorWrite));
        descriptorWrites.push_back(std::move(samplerDescriptorWrite));

        vkUpdateDescriptorSets(globals.device.handle, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
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

    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.handle, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer.handle, 0, VK_INDEX_TYPE_UINT16);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, globals.graphicsPipeline.layout, 0, 1, &descriptorSets[frameIndex], 0, nullptr);
    vkCmdDraw(commandBuffer, vertices.size(), 1, 0, 0);
    // vkCmdDrawIndexed(commandBuffer, indices.size(), 1, 0, 0, 0);
    vkCmdEndRenderPass(commandBuffer);

    THROW_IF_FAILED(
        vkEndCommandBuffer(commandBuffer),
        __FILE__, __LINE__,
        "Failed to end command buffer");
}

void Cubes::updateUniformBuffer(u32 frameIndex)
{
    UniformBufferObject ubo = {};
    ubo.world = glm::mat4(1.0f);
    ubo.view = camera.matrices.view;
    ubo.proj = camera.matrices.proj;
    memcpy(uniformBuffers[frameIndex].mapped, &ubo, sizeof(ubo));
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
    vkDestroyImageView(globals.device.handle, texture.view, globals.allocator);
    destroyImage(globals, texture);
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

void Cubes::destroyDescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(globals.device.handle, globals.graphicsPipeline.descriptorSetLayout, globals.allocator);
    LOG_DEBUG("Descriptor set layout destroyed");
}

std::unique_ptr<SampleBase> createSample()
{
    return std::make_unique<Cubes>(1600, 900, "Cubes");
}
