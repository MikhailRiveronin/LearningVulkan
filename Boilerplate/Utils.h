#pragma once

#include "Defines.h"
#include "Logger.h"
#include "Structures.h"

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

#define THROW_IF_FAILED(result, file, line, message)                          \
    do  {                                                                     \
        if (result != VK_SUCCESS) {                                           \
            LOG_ERROR("%s (%u): %s", file, line, message);                    \
            throw std::runtime_error(message);                                \
        }                                                                     \
    } while (0)

void createBuffer(Globals const& globals, Buffer& buffer);
void destroyBuffer(Globals const& globals, Buffer& buffer);
void copyBuffer(Globals const& globals, Buffer& srcBuffer, Buffer& dstBuffer);
void copyBufferToImage(Globals const& globals, Buffer& buffer, Image& image);
VkDeviceSize calculateUniformBufferAlignment(Globals const& globals, VkDeviceSize size);

std::vector<char> loadShaderCode(std::string const& filename);

void createImage(Globals const& globals, Image& image);
void destroyImage(Globals const& globals, Image const& image);
void createImageView(Globals const& globals, Image& image);
void transitionImageLayout(Globals const& globals, Image& image, VkImageLayout oldLayout, VkImageLayout newLayout);

void createSampler(Globals const& globals, Sampler& sampler);

u32 findMemoryType(Globals const& globals, VkMemoryRequirements const& requirements, VkMemoryPropertyFlags properties);

VkCommandBuffer beginCommandBufferOneTimeSubmit(Globals const& globals);
void endCommandBufferOneTimeSubmit(Globals const& globals, VkCommandBuffer commandBuffer);

void createDescriptorSets(Globals const& globals, std::vector<DescriptorSetBinding> const& descriptorSetBindings, DescriptorSets& descriptorSets);
void destroyDescriptorSets(Globals const& globals, DescriptorSets& descriptorSets);

void createPipeline(
    Globals const& globals,
    std::unordered_map<VkShaderStageFlagBits, std::string> shaderStages,
    VertexInputState const& vertexInputState,
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts,
    std::vector<VkPushConstantRange> pushConstantRanges,
    std::vector<PipelineCreateInfo> const& pipelineCreateInfos);
void destroyPipeline(Globals const& globals, Pipeline& pipeline);
