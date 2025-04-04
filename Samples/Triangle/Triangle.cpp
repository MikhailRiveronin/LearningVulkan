#include "Boilerplate/Entry.h"

#include "Boilerplate/Types.h"
#include "Boilerplate/Utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <chrono>

static VkShaderModule createShaderModule(Context& context, const std::vector<char>& code);

class Triangle : public SampleBase {
public:
    Triangle(u32 width, u32 height, std::string const& name);

    void createDescriptorSetLayout() override;
    void createGraphicsPipeline() override;
    void createUniformBuffers() override;
    void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex) override;

private:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 texCoords;

        static VkVertexInputBindingDescription getBindingDescription();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescription();
    };

    struct UniformBufferObject {
        glm::mat4 world;
        glm::mat4 view;
        glm::mat4 proj;
    };

    std::vector<Vertex> vertices;
    std::vector<u16> indices;
    Buffer vertexBuffer;
    Buffer indexBuffer;

    Image texture;
    Sampler sampler;

    void createGeometry() override;
    void createVertexBuffer() override;
    void createIndexBuffer() override;
    void sampleSpecificShutdown() override;

    void updateUniformBuffer(u32 frameIndex) override;
    void createDescriptorPool() override;
    void allocateDescriptorSets() override;

    void createTextureImage() override;
};

Triangle::Triangle(u32 width, u32 height, std::string const& name) :
    SampleBase(width, height, name)
{
}

void Triangle::createDescriptorSetLayout()
{
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

    VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    layoutBindings.push_back(std::move(uboLayoutBinding));

    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    samplerLayoutBinding.pImmutableSamplers = nullptr;

    layoutBindings.push_back(std::move(samplerLayoutBinding));

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.bindingCount = layoutBindings.size();
    createInfo.pBindings = layoutBindings.data();

    THROW_IF_FAILED(vkCreateDescriptorSetLayout(context.device.handle, &createInfo, context.allocator, &context.graphicsPipeline.descriptorSetLayout), __FILE__, __LINE__, "Failed to create descriptor set layout");
}

void Triangle::createGraphicsPipeline()
{
    auto vertexShaderCode = loadShaderBinary("Shaders/Vertex.spv");
    auto fragmentShaderCode = loadShaderBinary("Shaders/Fragment.spv");

    auto vertexShaderModule = createShaderModule(context, vertexShaderCode);
    auto fragmentShaderModule = createShaderModule(context, fragmentShaderCode);

    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;

    VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = {};
    vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageCreateInfo.pNext = nullptr;
    vertexShaderStageCreateInfo.flags = 0;
    vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageCreateInfo.module = vertexShaderModule;
    vertexShaderStageCreateInfo.pName = "main";
    vertexShaderStageCreateInfo.pSpecializationInfo = nullptr;

    shaderStageCreateInfos.push_back(std::move(vertexShaderStageCreateInfo));

    VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = {};
    fragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageCreateInfo.pNext = nullptr;
    fragmentShaderStageCreateInfo.flags = 0;
    fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT ;
    fragmentShaderStageCreateInfo.module = fragmentShaderModule;
    fragmentShaderStageCreateInfo.pName = "main";
    fragmentShaderStageCreateInfo.pSpecializationInfo = nullptr;

    shaderStageCreateInfos.push_back(std::move(fragmentShaderStageCreateInfo));

    VkVertexInputBindingDescription vertexInputBindingDescription = {};
    vertexInputBindingDescription.binding = 0;
    vertexInputBindingDescription.stride = sizeof(Vertex);
    vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;

    VkVertexInputAttributeDescription positionVertexInputAttributeDescription = {};
    positionVertexInputAttributeDescription.location = 0;
    positionVertexInputAttributeDescription.binding = 0;
    positionVertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    positionVertexInputAttributeDescription.offset = offsetof(Vertex, position);

    vertexInputAttributeDescriptions.push_back(std::move(positionVertexInputAttributeDescription));

    VkVertexInputAttributeDescription colorVertexInputAttributeDescription = {};
    colorVertexInputAttributeDescription.location = 1;
    colorVertexInputAttributeDescription.binding = 0;
    colorVertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    colorVertexInputAttributeDescription.offset = offsetof(Vertex, color);

    vertexInputAttributeDescriptions.push_back(std::move(colorVertexInputAttributeDescription));

    VkVertexInputAttributeDescription texCoordsVertexInputAttributeDescription = {};
    texCoordsVertexInputAttributeDescription.location = 2;
    texCoordsVertexInputAttributeDescription.binding = 0;
    texCoordsVertexInputAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
    texCoordsVertexInputAttributeDescription.offset = offsetof(Vertex, texCoords);

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
    viewport.width = context.swapchain.extent.width;
    viewport.height = context.swapchain.extent.height;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = context.swapchain.extent;

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
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
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

    VkPipelineLayoutCreateInfo layoutCreateInfo = {};
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutCreateInfo.pNext = nullptr;
    layoutCreateInfo.flags = 0;
    layoutCreateInfo.setLayoutCount = 1;
    layoutCreateInfo.pSetLayouts = &context.graphicsPipeline.descriptorSetLayout;
    layoutCreateInfo.pushConstantRangeCount = 0;
    layoutCreateInfo.pPushConstantRanges = nullptr;

    THROW_IF_FAILED(vkCreatePipelineLayout(context.device.handle, &layoutCreateInfo, context.allocator, &context.graphicsPipeline.layout), __FILE__, __LINE__, "Failed to create pipeline layout");

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.pNext = nullptr;
    dynamicStateCreateInfo.flags = 0;
    dynamicStateCreateInfo.dynamicStateCount = 0;
    dynamicStateCreateInfo.pDynamicStates = nullptr;

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
    createInfo.pDynamicState = nullptr;
    createInfo.layout = context.graphicsPipeline.layout;
    createInfo.renderPass = context.renderPass;
    createInfo.subpass = 0;
    createInfo.basePipelineHandle = VK_NULL_HANDLE;
    createInfo.basePipelineIndex = 0;

    THROW_IF_FAILED(vkCreateGraphicsPipelines(context.device.handle, VK_NULL_HANDLE, 1, &createInfo, context.allocator, &context.graphicsPipeline.pipeline), __FILE__, __LINE__, "Failed to create graphics pipeline");

    vkDestroyShaderModule(context.device.handle, vertexShaderModule, context.allocator);
    vkDestroyShaderModule(context.device.handle, fragmentShaderModule, context.allocator);
}

void Triangle::createUniformBuffers()
{
    context.uniformBuffers.resize(framesInFlight);

    for (auto& uniformBuffer : context.uniformBuffers) {
        uniformBuffer.size = sizeof(UniformBufferObject);
        uniformBuffer.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        uniformBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        createBuffer(context, uniformBuffer);

        THROW_IF_FAILED(vkMapMemory(context.device.handle, uniformBuffer.memory, 0, uniformBuffer.size, 0, &uniformBuffer.mapped), __FILE__, __LINE__, "Failed to map memory");
    }
}

void Triangle::recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, u32 frameIndex)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    THROW_IF_FAILED(vkBeginCommandBuffer(commandBuffer, &beginInfo), __FILE__, __LINE__, "Failed to begin command buffer");

    std::vector<VkClearValue> clearValues;

    VkClearValue clearColor = {};
    clearColor.color = {{ 0.f, 0.f, 0.f, 1.f }};

    clearValues.push_back(std::move(clearColor));

    VkClearValue clearDepth = {};
    clearDepth.depthStencil = { 1.0f, 0 };

    clearValues.push_back(std::move(clearDepth));

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = context.renderPass;
    renderPassBeginInfo.framebuffer = context.swapchain.framebuffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    renderPassBeginInfo.renderArea.extent = context.swapchain.extent;
    renderPassBeginInfo.clearValueCount = clearValues.size();
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context.graphicsPipeline.pipeline);

    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.handle, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer.handle, 0, VK_INDEX_TYPE_UINT16);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context.graphicsPipeline.layout, 0, 1, &context.descriptorSets[frameIndex], 0, nullptr);

    // vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    vkCmdDrawIndexed(commandBuffer, indices.size(), 1, 0, 0, 0);
    vkCmdEndRenderPass(commandBuffer);


    THROW_IF_FAILED(vkEndCommandBuffer(commandBuffer), __FILE__, __LINE__, "Failed to end command buffer");
}

std::unique_ptr<SampleBase> createSample()
{
    return std::make_unique<Triangle>(1280, 720, "Triangle");
}

VkVertexInputBindingDescription Triangle::Vertex::getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription;
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> Triangle::Vertex::getAttributeDescription()
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
}

void Triangle::createGeometry()
{
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
    };

    this->vertices = vertices;

    const std::vector<u16> indices = {  0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 };
    this->indices = indices;
}

void Triangle::createVertexBuffer()
{
    Buffer stagingBuffer;
    stagingBuffer.size = vertices.size() * sizeof(vertices[0]);
    stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    createBuffer(context, stagingBuffer);

    THROW_IF_FAILED(vkMapMemory(context.device.handle, stagingBuffer.memory, 0, stagingBuffer.size, 0, &stagingBuffer.mapped), __FILE__, __LINE__, "Failed to map memory");
    memcpy(stagingBuffer.mapped, vertices.data(), stagingBuffer.size);
    vkUnmapMemory(context.device.handle, stagingBuffer.memory);

    vertexBuffer.size = stagingBuffer.size;
    vertexBuffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    createBuffer(context, vertexBuffer);

    copyBuffer(context, stagingBuffer, vertexBuffer);

    destroyBuffer(context, stagingBuffer);
}

void Triangle::createIndexBuffer()
{
    Buffer stagingBuffer;
    stagingBuffer.size = indices.size() * sizeof(indices[0]);
    stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    createBuffer(context, stagingBuffer);

    THROW_IF_FAILED(vkMapMemory(context.device.handle, stagingBuffer.memory, 0, stagingBuffer.size, 0, &stagingBuffer.mapped), __FILE__, __LINE__, "Failed to map memory");
    memcpy(stagingBuffer.mapped, indices.data(), stagingBuffer.size);
    vkUnmapMemory(context.device.handle, stagingBuffer.memory);

    indexBuffer.size = stagingBuffer.size;
    indexBuffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    indexBuffer.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    createBuffer(context, indexBuffer);

    copyBuffer(context, stagingBuffer, indexBuffer);

    destroyBuffer(context, stagingBuffer);
}

void Triangle::sampleSpecificShutdown()
{
    vkDestroyDescriptorPool(context.device.handle, context.descriptorPool, context.allocator);

    vkDestroyDescriptorSetLayout(context.device.handle, context.graphicsPipeline.descriptorSetLayout, context.allocator);

    for (u32 i = 0; i < framesInFlight; ++i) {
        vkUnmapMemory(context.device.handle, context.uniformBuffers[i].memory);
        destroyBuffer(context, context.uniformBuffers[i]);
    }

    destroyBuffer(context, indexBuffer);
    destroyBuffer(context, vertexBuffer);

    vkDestroyImageView(context.device.handle, texture.view, context.allocator);
    destroyImage(context, texture);
}

void Triangle::updateUniformBuffer(u32 frameIndex)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo = {};
    ubo.world = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), (float)context.swapchain.extent.width / (float)context.swapchain.extent.height, 0.1f, 10.0f);

    memcpy(context.uniformBuffers[frameIndex].mapped, &ubo, sizeof(ubo));
}

void Triangle::createDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> poolSizes;

    VkDescriptorPoolSize uboPoolSize = {};
    uboPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboPoolSize.descriptorCount = framesInFlight;

    poolSizes.push_back(std::move(uboPoolSize));

    VkDescriptorPoolSize samplerPoolSize = {};
    samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerPoolSize.descriptorCount = framesInFlight;

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.maxSets = framesInFlight;
    createInfo.poolSizeCount = poolSizes.size();
    createInfo.pPoolSizes = poolSizes.data();

    THROW_IF_FAILED(vkCreateDescriptorPool(context.device.handle, &createInfo, context.allocator, &context.descriptorPool), __FILE__, __LINE__, "Failed to create descriptor pool");
}

void Triangle::allocateDescriptorSets()
{
    context.descriptorSets.resize(framesInFlight);

    std::vector<VkDescriptorSetLayout> setLayouts(framesInFlight, context.graphicsPipeline.descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.descriptorPool = context.descriptorPool;
    allocateInfo.descriptorSetCount = framesInFlight;
    allocateInfo.pSetLayouts = setLayouts.data();

    THROW_IF_FAILED(vkAllocateDescriptorSets(context.device.handle, &allocateInfo, context.descriptorSets.data()), __FILE__, __LINE__, "Failed to allocate descriptor sets");

    for (u32 i = 0; i < framesInFlight; ++i) {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = context.uniformBuffers[i].handle;
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;

        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = sampler.handle;
        imageInfo.imageView = texture.view;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        std::vector<VkWriteDescriptorSet> descriptorWrites;

        VkWriteDescriptorSet uboDescriptorWrite = {};
        uboDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uboDescriptorWrite.pNext = nullptr;
        uboDescriptorWrite.dstSet = context.descriptorSets[i];
        uboDescriptorWrite.dstBinding = 0;
        uboDescriptorWrite.dstArrayElement = 0;
        uboDescriptorWrite.descriptorCount = 1;
        uboDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboDescriptorWrite.pImageInfo = nullptr;
        uboDescriptorWrite.pBufferInfo = &bufferInfo;
        uboDescriptorWrite.pTexelBufferView = nullptr;

        descriptorWrites.push_back(std::move(uboDescriptorWrite));

        VkWriteDescriptorSet samplerDescriptorWrite = {};
        samplerDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        samplerDescriptorWrite.pNext = nullptr;
        samplerDescriptorWrite.dstSet = context.descriptorSets[i];
        samplerDescriptorWrite.dstBinding = 1;
        samplerDescriptorWrite.dstArrayElement = 0;
        samplerDescriptorWrite.descriptorCount = 1;
        samplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerDescriptorWrite.pImageInfo = &imageInfo;
        samplerDescriptorWrite.pBufferInfo = nullptr;
        samplerDescriptorWrite.pTexelBufferView = nullptr;

        descriptorWrites.push_back(std::move(samplerDescriptorWrite));

        vkUpdateDescriptorSets(context.device.handle, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }
}

void Triangle::createTextureImage()
{
    int width;
    int height;
    int channels;
    stbi_uc* pixels = stbi_load("Textures/texture.jpg", &width, &height, &channels, STBI_rgb_alpha);
    VkDeviceSize imageSize = width * height * 4;

    if (!pixels) {
        throw std::runtime_error("Failed to load texture image");
    }

    Buffer stagingBuffer;
    stagingBuffer.size = imageSize;
    stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    createBuffer(context, stagingBuffer);

    vkMapMemory(context.device.handle, stagingBuffer.memory, 0, stagingBuffer.size, 0, &stagingBuffer.mapped);
    memcpy(stagingBuffer.mapped, pixels, imageSize);
    vkUnmapMemory(context.device.handle, stagingBuffer.memory);

    stbi_image_free(pixels);

    texture.width = width;
    texture.height = height;
    texture.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    texture.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    createImage(context, texture);

    transitionImageLayout(context, texture, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(context, stagingBuffer, texture);
    destroyBuffer(context, stagingBuffer);
    transitionImageLayout(context, texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    createImageView(context, texture);

    createSampler(context, sampler);
}
