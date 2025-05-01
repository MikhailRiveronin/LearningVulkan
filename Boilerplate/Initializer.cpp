#include "Initializer.h"
#include "Utils.h"

VkShaderModuleCreateInfo Initializer::shaderModuleCreateInfo(std::vector<char> const& code)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<u32 const*>(code.data());
    return createInfo;
}

VkVertexInputBindingDescription Initializer::vertexInputBindingDescription(u32 binding, u32 stride, VkVertexInputRate inputRate)
{
    VkVertexInputBindingDescription description = {};
    description.binding = binding;
    description.stride = stride;
    description.inputRate = inputRate;
    return description;
}

VkVertexInputAttributeDescription Initializer::vertexInputAttributeDescription(u32 location, u32 binding, VkFormat format, u32 offset)
{
    VkVertexInputAttributeDescription description = {};
    description.location = location;
    description.binding = binding;
    description.format = format;
    description.offset = offset;
    return description;
}

VkViewport Initializer::viewport(u32 width, u32 height)
{
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(width);
    viewport.height = static_cast<float>(height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
    return viewport;
}

VkRect2D Initializer::scissor(u32 width, u32 height)
{
    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = { width, height };
    return scissor;
}

VkDescriptorPoolSize Initializer::descriptorPoolSize(VkDescriptorType type, u32 descriptorCount)
{
    VkDescriptorPoolSize poolSize = {};
    poolSize.type = type;
    poolSize.descriptorCount = descriptorCount;
    return poolSize;
}

VkDescriptorPoolCreateInfo Initializer::descriptorPoolCreateInfo(u32 maxSets, std::vector<VkDescriptorPoolSize> const& poolSizes)
{
    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.maxSets = maxSets;
    createInfo.poolSizeCount = poolSizes.size();
    createInfo.pPoolSizes = poolSizes.data();
    return createInfo;
}

VkDescriptorSetLayoutBinding Initializer::descriptorSetLayoutBinding(
    u32 binding,
    VkDescriptorType descriptorType,
    u32 descriptorCount,
    VkShaderStageFlags stageFlags)
{
    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = descriptorCount;
    layoutBinding.stageFlags = stageFlags;
    layoutBinding.pImmutableSamplers = nullptr;
    return layoutBinding;
}

VkDescriptorSetLayoutBindingFlagsCreateInfo Initializer::descriptorSetLayoutBindingFlagsCreateInfo(std::vector<VkDescriptorBindingFlags> const& bindingFlags)
{
    VkDescriptorSetLayoutBindingFlagsCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.bindingCount = bindingFlags.size();
    createInfo.pBindingFlags = bindingFlags.data();
    return createInfo;
}

VkDescriptorSetLayoutCreateInfo Initializer::descriptorSetLayoutCreateInfo(std::vector<VkDescriptorSetLayoutBinding> const& bindings, void const* next)
{
    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext = next;
    createInfo.flags = 0;
    createInfo.bindingCount = bindings.size();
    createInfo.pBindings = bindings.data();
    return createInfo;
}

VkDescriptorSetVariableDescriptorCountAllocateInfo Initializer::descriptorSetVariableDescriptorCountAllocateInfo(std::vector<u32> const& descriptorCounts)
{
    VkDescriptorSetVariableDescriptorCountAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.descriptorSetCount = descriptorCounts.size();
    allocateInfo.pDescriptorCounts = descriptorCounts.data();
    return allocateInfo;
}

VkDescriptorSetAllocateInfo Initializer::descriptorSetAllocateInfo(
    VkDescriptorPool descriptorPool,
    u32 descriptorSetCount,
    std::vector<VkDescriptorSetLayout> const& setLayouts,
    void const* next)
{
    VkDescriptorSetAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.pNext = next;
    allocateInfo.descriptorPool = descriptorPool;
    allocateInfo.descriptorSetCount = descriptorSetCount;
    allocateInfo.pSetLayouts = setLayouts.data();
    return allocateInfo;
}

VkDescriptorBufferInfo Initializer::descriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
{
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = buffer;
    bufferInfo.offset = offset;
    bufferInfo.range = range;
    return bufferInfo;
}

VkDescriptorImageInfo Initializer::descriptorImageInfo(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout)
{
    VkDescriptorImageInfo imageInfo = {};
    imageInfo.sampler = sampler;
    imageInfo.imageView = imageView;
    imageInfo.imageLayout = imageLayout;
    return imageInfo;
}

VkWriteDescriptorSet Initializer::writeDescriptorSet(
    VkDescriptorSet dstSet,
    u32 dstBinding,
    u32 dstArrayElement,
    u32 descriptorCount,
    VkDescriptorType descriptorType,
    VkDescriptorImageInfo const* imageInfo,
    VkDescriptorBufferInfo const* bufferInfo,
    VkBufferView const* texelBufferView)
{
    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.pNext = nullptr;
    descriptorWrite.dstSet = dstSet;
    descriptorWrite.dstBinding = dstBinding;
    descriptorWrite.dstArrayElement = dstArrayElement;
    descriptorWrite.descriptorCount = descriptorCount;
    descriptorWrite.descriptorType = descriptorType;
    descriptorWrite.pImageInfo = imageInfo;
    descriptorWrite.pBufferInfo = bufferInfo;
    descriptorWrite.pTexelBufferView = texelBufferView;
    return descriptorWrite;
}

VkPipelineShaderStageCreateInfo Initializer::pipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shader)
{
    VkPipelineShaderStageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.stage = stage;
    createInfo.module = shader;
    createInfo.pName = "main";
    createInfo.pSpecializationInfo = nullptr;
    return createInfo;
}

VkPipelineVertexInputStateCreateInfo Initializer::pipelineVertexInputStateCreateInfo(
    std::vector<VkVertexInputBindingDescription> const& vertexBindingDescriptions,
    std::vector<VkVertexInputAttributeDescription> const& vertexAttributeDescriptions)
{
    VkPipelineVertexInputStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.vertexBindingDescriptionCount = vertexBindingDescriptions.size();
    createInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
    createInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptions.size();
    createInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
    return createInfo;
}

VkPipelineInputAssemblyStateCreateInfo Initializer::pipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable)
{
    VkPipelineInputAssemblyStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.topology = topology;
    createInfo.primitiveRestartEnable = primitiveRestartEnable;
    return createInfo;
}

VkPipelineTessellationStateCreateInfo Initializer::pipelineTessellationStateCreateInfo()
{
    VkPipelineTessellationStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.patchControlPoints = 0;
    return createInfo;
}

VkPipelineViewportStateCreateInfo Initializer::pipelineViewportStateCreateInfo(std::vector<VkViewport> const& viewports, std::vector<VkRect2D> const& scissors)
{
    VkPipelineViewportStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.viewportCount = viewports.size();
    createInfo.pViewports = viewports.data();
    createInfo.scissorCount = scissors.size();
    createInfo.pScissors = scissors.data();
    return createInfo;
}

VkPipelineRasterizationStateCreateInfo Initializer::pipelineRasterizationStateCreateInfo(
    VkBool32 depthClampEnable,
    VkBool32 rasterizerDiscardEnable,
    VkPolygonMode polygonMode,
    VkCullModeFlags cullMode,
    VkFrontFace frontFace,
    VkBool32 depthBiasEnable,
    float depthBiasConstantFactor,
    float depthBiasClamp,
    float depthBiasSlopeFactor,
    float lineWidth)
{
    VkPipelineRasterizationStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.depthClampEnable = depthClampEnable;
    createInfo.rasterizerDiscardEnable = rasterizerDiscardEnable;
    createInfo.polygonMode = polygonMode;
    createInfo.cullMode = cullMode;
    createInfo.frontFace = frontFace;
    createInfo.depthBiasEnable = depthBiasEnable;
    createInfo.depthBiasConstantFactor = depthBiasConstantFactor;
    createInfo.depthBiasClamp = depthBiasClamp;
    createInfo.depthBiasSlopeFactor = depthBiasSlopeFactor;
    createInfo.lineWidth = lineWidth;
    return createInfo;
}

VkPipelineMultisampleStateCreateInfo Initializer::pipelineMultisampleStateCreateInfo(
    VkSampleCountFlagBits rasterizationSamples,
    VkBool32 sampleShadingEnable,
    float minSampleShading,
    VkSampleMask const* sampleMask,
    VkBool32 alphaToCoverageEnable,
    VkBool32 alphaToOneEnable)
{
    VkPipelineMultisampleStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.rasterizationSamples = rasterizationSamples;
    createInfo.sampleShadingEnable = sampleShadingEnable;
    createInfo.minSampleShading = minSampleShading;
    createInfo.pSampleMask = sampleMask;
    createInfo.alphaToCoverageEnable = alphaToCoverageEnable;
    createInfo.alphaToOneEnable = alphaToOneEnable;
    return createInfo;
}

VkPipelineDepthStencilStateCreateInfo Initializer::pipelineDepthStencilStateCreateInfo(
    VkBool32 depthTestEnable,
    VkBool32 depthWriteEnable,
    VkCompareOp depthCompareOp,
    VkBool32 depthBoundsTestEnable,
    VkBool32 stencilTestEnable,
    VkStencilOpState front,
    VkStencilOpState back)
{
    VkPipelineDepthStencilStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.depthTestEnable = depthTestEnable;
    createInfo.depthWriteEnable = depthWriteEnable;
    createInfo.depthCompareOp = depthCompareOp;
    createInfo.depthBoundsTestEnable = depthBoundsTestEnable;
    createInfo.stencilTestEnable = stencilTestEnable;
    createInfo.front = front;
    createInfo.back = back;
    createInfo.minDepthBounds = 0.f;
    createInfo.maxDepthBounds = 1.f;
    return createInfo;
}

VkPipelineColorBlendAttachmentState Initializer::pipelineColorBlendAttachmentState(
    VkBool32 blendEnable,
    VkBlendFactor srcColorBlendFactor,
    VkBlendFactor dstColorBlendFactor,
    VkBlendOp colorBlendOp,
    VkBlendFactor srcAlphaBlendFactor,
    VkBlendFactor dstAlphaBlendFactor,
    VkBlendOp alphaBlendOp,
    VkColorComponentFlags colorWriteMask)
{
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.blendEnable = blendEnable;
    colorBlendAttachment.srcColorBlendFactor = srcColorBlendFactor;
    colorBlendAttachment.dstColorBlendFactor = dstColorBlendFactor;
    colorBlendAttachment.colorBlendOp = colorBlendOp;
    colorBlendAttachment.srcAlphaBlendFactor = srcAlphaBlendFactor;
    colorBlendAttachment.dstAlphaBlendFactor = dstAlphaBlendFactor;
    colorBlendAttachment.alphaBlendOp = alphaBlendOp;
    colorBlendAttachment.colorWriteMask = colorWriteMask;
    return colorBlendAttachment;
}

VkPipelineColorBlendStateCreateInfo Initializer::pipelineColorBlendStateCreateInfo(std::vector<VkPipelineColorBlendAttachmentState> const& colorBlendAttachments)
{
    VkPipelineColorBlendStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.logicOpEnable = VK_FALSE;
    createInfo.logicOp = VK_LOGIC_OP_CLEAR;
    createInfo.attachmentCount = colorBlendAttachments.size();
    createInfo.pAttachments = colorBlendAttachments.data();
    createInfo.blendConstants[0] = { 0.f };
    createInfo.blendConstants[1] = { 0.f };
    createInfo.blendConstants[2] = { 0.f };
    createInfo.blendConstants[3] = { 0.f };
    return createInfo;
}

VkPipelineDynamicStateCreateInfo Initializer::pipelineDynamicStateCreateInfo(std::vector<VkDynamicState> const& dynamicStates)
{
    VkPipelineDynamicStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.dynamicStateCount = dynamicStates.size();
    createInfo.pDynamicStates = dynamicStates.data();
    return createInfo;
}

VkPipelineLayoutCreateInfo Initializer::pipelineLayoutCreateInfo(
    std::vector<VkDescriptorSetLayout> const& setLayouts,
    std::vector<VkPushConstantRange> const& pushConstantRanges)
{
    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.setLayoutCount = setLayouts.size();
    createInfo.pSetLayouts = setLayouts.data();
    createInfo.pushConstantRangeCount = pushConstantRanges.size();
    createInfo.pPushConstantRanges = pushConstantRanges.data();
    return createInfo;
}

VkGraphicsPipelineCreateInfo Initializer::graphicsPipelineCreateInfo(
    std::vector<VkPipelineShaderStageCreateInfo> const& stages,
    VkPipelineVertexInputStateCreateInfo const* vertexInputState,
    VkPipelineInputAssemblyStateCreateInfo const* inputAssemblyState,
    VkPipelineTessellationStateCreateInfo const* tessellationState,
    VkPipelineViewportStateCreateInfo const* viewportState,
    VkPipelineRasterizationStateCreateInfo const* rasterizationState,
    VkPipelineMultisampleStateCreateInfo const* multisampleState,
    VkPipelineDepthStencilStateCreateInfo const* depthStencilState,
    VkPipelineColorBlendStateCreateInfo const* colorBlendState,
    VkPipelineDynamicStateCreateInfo const* dynamicState,
    VkPipelineLayout layout,
    VkRenderPass renderPass,
    u32 subpass,
    VkPipeline basePipelineHandle,
    i32 basePipelineIndex)
{
    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.stageCount = stages.size();
    createInfo.pStages = stages.data();
    createInfo.pVertexInputState = vertexInputState;
    createInfo.pInputAssemblyState = inputAssemblyState;
    createInfo.pTessellationState = tessellationState;
    createInfo.pViewportState = viewportState;
    createInfo.pRasterizationState = rasterizationState;
    createInfo.pMultisampleState = multisampleState;
    createInfo.pDepthStencilState = depthStencilState;
    createInfo.pColorBlendState = colorBlendState;
    createInfo.pDynamicState = dynamicState;
    createInfo.layout = layout;
    createInfo.renderPass = renderPass;
    createInfo.subpass = subpass;
    createInfo.basePipelineHandle = basePipelineHandle;
    createInfo.basePipelineIndex = basePipelineIndex;
    return createInfo;
}

VkCommandBufferBeginInfo Initializer::commandBufferBeginInfo()
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;
    return beginInfo;
}

VkRenderPassBeginInfo Initializer::renderPassBeginInfo(
    VkRenderPass renderPass,
    VkFramebuffer framebuffer,
    VkRect2D renderArea,
    std::vector<VkClearValue> const& clearValues)
{
    VkRenderPassBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.renderPass = renderPass;
    beginInfo.framebuffer = framebuffer;
    beginInfo.renderArea = renderArea;
    beginInfo.clearValueCount = clearValues.size();
    beginInfo.pClearValues = clearValues.data();
    return beginInfo;
}
