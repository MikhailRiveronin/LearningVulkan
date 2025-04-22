#pragma once

#include "Defines.h"
#include "Structures.h"

#include <vulkan/vulkan.h>
#include <vector>

class Initializer {
public:
    static VkShaderModuleCreateInfo shaderModuleCreateInfo(std::vector<char> const& code);
    static VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shader);
    static VkVertexInputBindingDescription vertexInputBindingDescription(u32 binding, u32 stride, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
    static VkVertexInputAttributeDescription vertexInputAttributeDescription(u32 location, u32 binding, VkFormat format, u32 offset);
    static VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(
        std::vector<VkVertexInputBindingDescription> const& vertexBindingDescriptions,
        std::vector<VkVertexInputAttributeDescription> const& vertexAttributeDescriptions);
    static VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable = VK_FALSE);
    static VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo();
    static VkViewport viewport(float width, float height);
    static VkRect2D scissor(u32 width, u32 height);
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
    static VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(
        u32 binding,
        VkDescriptorType descriptorType,
        u32 descriptorCount,
        VkShaderStageFlags stageFlags);
    static VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(std::vector<VkDescriptorSetLayoutBinding> const& bindings);
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
};
