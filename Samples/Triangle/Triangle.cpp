#include "Boilerplate/Entry.h"

#include "Boilerplate/Types.h"
#include "Boilerplate/Utils.h"

#include <glm/glm.hpp>

static VkShaderModule createShaderModule(Context& context, const std::vector<char>& code);

class Triangle : public SampleBase {
public:
    Triangle(u32 width, u32 height, std::string const& name);

    void createGraphicsPipeline() override;
    void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex) override;

private:
    struct Vertex {
        glm::vec2 position;
        glm::vec3 color;

        static VkVertexInputBindingDescription getBindingDescription();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescription();
    };

    std::vector<Vertex> vertices;
    Buffer vertexBuffer;

    void createGeometry() override;
    void createVertexBuffer() override;
    void sampleSpecificShutdown() override;
};

Triangle::Triangle(u32 width, u32 height, std::string const& name) :
    SampleBase(width, height, name)
{
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

    VkVertexInputAttributeDescription vertexInputAttributeDescription[2] = {};
    vertexInputAttributeDescription[0].location = 0;
    vertexInputAttributeDescription[0].binding = 0;
    vertexInputAttributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
    vertexInputAttributeDescription[0].offset = offsetof(Vertex, position);
    vertexInputAttributeDescription[1].location = 1;
    vertexInputAttributeDescription[1].binding = 0;
    vertexInputAttributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributeDescription[1].offset = offsetof(Vertex, color);

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.pNext = nullptr;
    vertexInputStateCreateInfo.flags = 0;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2;
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescription;

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
    layoutCreateInfo.setLayoutCount = 0;
    layoutCreateInfo.pSetLayouts = nullptr;
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
    createInfo.pDepthStencilState = nullptr;
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

void Triangle::recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    THROW_IF_FAILED(vkBeginCommandBuffer(commandBuffer, &beginInfo), __FILE__, __LINE__, "Failed to begin command buffer");

    VkClearValue clearColor = {};
    clearColor.color = {{ 0.f, 0.f, 0.f, 1.f }};

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = context.renderPass;
    renderPassBeginInfo.framebuffer = context.swapchain.framebuffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    renderPassBeginInfo.renderArea.extent = context.swapchain.extent;
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context.graphicsPipeline.pipeline);

    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.handle, offsets);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
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
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

    this->vertices = vertices;
}

void Triangle::createVertexBuffer()
{
    vertexBuffer.size = vertices.size() * sizeof(vertices[0]);
    vertexBuffer.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    createBuffer(context, vertexBuffer);

    THROW_IF_FAILED(vkMapMemory(context.device.handle, vertexBuffer.memory, 0, vertexBuffer.size, 0, &vertexBuffer.mapped), __FILE__, __LINE__, "Failed to map memory");
    memcpy(vertexBuffer.mapped, vertices.data(), vertexBuffer.size);
    vkUnmapMemory(context.device.handle, vertexBuffer.memory);
}

void Triangle::sampleSpecificShutdown()
{
    destroyBuffer(context, vertexBuffer);
}
