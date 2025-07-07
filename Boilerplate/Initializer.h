#pragma once

#include "Defines.h"
#include "Structures.h"

#include <vulkan/vulkan.h>
#include <vector>

class Initializer {
public:
    static VkAttachmentDescription attachmentDescription(
        VkFormat format,
        VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
        VkImageLayout initialLayout, VkImageLayout finalLayout,
        VkAttachmentDescriptionFlags flags = 0,
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
        VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);
    static VkAttachmentReference attachmentReference(u32 attachment, VkImageLayout layout);
    static VkSubpassDescription subpassDescription(
        std::vector<VkAttachmentReference> const& colorAttachments,
        VkAttachmentReference const* depthStencilAttachment,
        VkSubpassDescriptionFlags flags = 0,
        VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        std::vector<VkAttachmentReference> const& inputAttachments = std::vector<VkAttachmentReference>(),
        std::vector<VkAttachmentReference> const& resolveAttachments = std::vector<VkAttachmentReference>(),
        std::vector<u32> const& preserveAttachments = std::vector<u32>());
    static VkSubpassDependency subpassDependency(
        u32 srcSubpass, u32 dstSubpass,
        VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
        VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
        VkDependencyFlags dependencyFlags = 0);
    static VkRenderPassCreateInfo renderPassCreateInfo(
        std::vector<VkAttachmentDescription> const& attachments,
        std::vector<VkSubpassDescription> const& subpasses,
        std::vector<VkSubpassDependency> const& dependencies);

    static VkFramebufferCreateInfo framebufferCreateInfo(
        VkRenderPass renderPass,
        std::vector<VkImageView> const& attachments,
        VkExtent2D const& extent,
        u32 layers = 1);


    static VkShaderModuleCreateInfo shaderModuleCreateInfo(std::vector<u32> const& code);

    static VkVertexInputBindingDescription vertexInputBindingDescription(u32 binding, u32 stride, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
    static VkVertexInputAttributeDescription vertexInputAttributeDescription(u32 location, u32 binding, VkFormat format, u32 offset);

    static VkViewport viewport(VkExtent2D extent);
    static VkRect2D scissor(VkExtent2D extent);

    static VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(u32 binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags);
    static VkDescriptorPoolSize descriptorPoolSize(VkDescriptorType type, u32 descriptorCount);
    static VkDescriptorPoolCreateInfo descriptorPoolCreateInfo(u32 maxSets, std::vector<VkDescriptorPoolSize> const& poolSizes);
    static VkDescriptorSetLayoutBindingFlagsCreateInfo descriptorSetLayoutBindingFlagsCreateInfo(std::vector<VkDescriptorBindingFlags> const& bindingFlags);
    static VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(std::vector<VkDescriptorSetLayoutBinding> const& bindings, void const* next = nullptr);
    static VkDescriptorSetVariableDescriptorCountAllocateInfo descriptorSetVariableDescriptorCountAllocateInfo(std::vector<u32> const& descriptorCounts);
    static VkDescriptorSetAllocateInfo descriptorSetAllocateInfo(
        VkDescriptorPool descriptorPool,
        u32 descriptorSetCount,
        std::vector<VkDescriptorSetLayout> const& setLayouts,
        void const* next = nullptr);
    static VkDescriptorBufferInfo descriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range = VK_WHOLE_SIZE);
    static VkDescriptorImageInfo descriptorImageInfo(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout);
    static VkWriteDescriptorSet writeDescriptorSet(
        VkDescriptorSet dstSet,
        u32 dstBinding,
        VkDescriptorType descriptorType,
        VkDescriptorImageInfo const* imageInfo,
        VkDescriptorBufferInfo const* bufferInfo);

    static VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module);
    static VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(
        std::vector<VkVertexInputBindingDescription> const& vertexBindingDescriptions,
        std::vector<VkVertexInputAttributeDescription> const& vertexAttributeDescriptions);
    static VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        VkBool32 primitiveRestartEnable = VK_FALSE);
    static VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo();
    static VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(std::vector<VkViewport> const& viewports, std::vector<VkRect2D> const& scissors);
    static VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(
        VkBool32 depthClampEnable = VK_FALSE,
        VkBool32 rasterizerDiscardEnable = VK_FALSE,
        VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
        VkCullModeFlags cullMode = VK_CULL_MODE_NONE,
        VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        VkBool32 depthBiasEnable = VK_FALSE,
        float depthBiasConstantFactor = 0.f,
        float depthBiasClamp = 0.f,
        float depthBiasSlopeFactor = 0.f,
        float lineWidth = 1.f);
    static VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(
        VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        VkBool32 sampleShadingEnable = VK_FALSE,
        float minSampleShading = 0.f,
        VkSampleMask const* sampleMask = nullptr,
        VkBool32 alphaToCoverageEnable = VK_FALSE,
        VkBool32 alphaToOneEnable = VK_FALSE);
    static VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(
        VkBool32 depthTestEnable = VK_TRUE,
        VkBool32 depthWriteEnable = VK_TRUE,
        VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS,
        VkBool32 depthBoundsTestEnable = VK_FALSE,
        VkBool32 stencilTestEnable = VK_FALSE,
        VkStencilOpState front = {},
        VkStencilOpState back = {});
    static VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(
        VkBool32 blendEnable = VK_FALSE,
        VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
        VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD,
        VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
    static VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(std::vector<VkPipelineColorBlendAttachmentState> const& colorBlendAttachments);
    static VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(std::vector<VkDynamicState> const& dynamicStates);
    static VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(
        std::vector<VkDescriptorSetLayout> const& setLayouts,
        std::vector<VkPushConstantRange> const& pushConstantRanges);
    static VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo(
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
        u32 subpass = 0,
        VkPipeline basePipelineHandle = VK_NULL_HANDLE,
        i32 basePipelineIndex = 0);

    static VkCommandBufferBeginInfo commandBufferBeginInfo();
    static VkRenderPassBeginInfo renderPassBeginInfo(
        VkRenderPass renderPass,
        VkFramebuffer framebuffer,
        VkRect2D renderArea,
        std::vector<VkClearValue> const& clearValues = std::vector<VkClearValue>());
};
