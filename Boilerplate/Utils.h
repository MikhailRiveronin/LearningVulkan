#pragma once

#include "Defines.h"
#include "Logger.h"
#include "Structures.h"

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

#define THROW_IF_FAILED(result)                                \
    do {                                                       \
        if (result != VK_SUCCESS) {                            \
            throw std::runtime_error("Vulkan command failed"); \
        }                                                      \
    } while (0)

void createBuffer(Context const& globals, Buffer& buffer);
void createImage(Context const& globals, Image& image);

void destroyBuffer(Context const& globals, Buffer& buffer);
void destroyImage(Context const& globals, Image const& image);

void copyBuffer(Context const& globals, Buffer& srcBuffer, Buffer& dstBuffer);
void copyBufferToImage(Context const& globals, Buffer& buffer, Image& image);
VkDeviceSize calculateUniformBufferAlignment(Context const& globals, VkDeviceSize size);

std::vector<char> loadShaderCode(std::string const& filename);

void createImageView(Context const& globals, Image& image);
void transitionImageLayout(Context const& globals, Image& image, VkImageLayout oldLayout, VkImageLayout newLayout);


u32 findMemoryTypeIndex(Context const& context, VkMemoryRequirements const& requirements, VkMemoryPropertyFlags properties);

VkCommandBuffer beginCommandBufferOneTimeSubmit(Context const& globals);
void endCommandBufferOneTimeSubmit(Context const& globals, VkCommandBuffer commandBuffer);

void createDescriptorSets(Context const& globals, std::vector<DescriptorSetBinding> const& descriptorSetBindings, DescriptorSets& descriptorSets);
void destroyDescriptorSets(Context const& globals, DescriptorSets& descriptorSets);

void createPipeline(
    Context const& globals,
    std::unordered_map<VkShaderStageFlagBits, std::string> shaderStages,
    VertexInputState const& vertexInputState,
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts,
    std::vector<VkPushConstantRange> pushConstantRanges,
    std::vector<PipelineCreateInfo> const& pipelineCreateInfos);
void destroyPipeline(Context const& globals, Pipeline& pipeline);

void createShaderStage(Context const& globals, ShaderStage shaderStage);

void createCubeTexture(
    Context const& context,
    char const* front, char const* back,
    char const* left, char const* right,
    char const* up, char const* down,
    Image& image);
