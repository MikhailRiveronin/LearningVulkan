#pragma once

#include "Defines.h"
#include "Logger.h"
#include "Types.h"

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <string>
#include <vector>

#define THROW_IF_FAILED(result, file, line, message)                          \
    do  {                                                                     \
        if (result != VK_SUCCESS) {                                           \
            LOG_ERROR("%s (%u): %s\n", file, line, message);                  \
            throw std::runtime_error(message);                                \
        }                                                                     \
    } while (0)

std::vector<u8> loadShaderBinary(std::string const& filename);
VkShaderModule createShaderModule(Context& context, std::vector<u8> const& code);

void createBuffer(Context& context, Buffer& buffer);
void destroyBuffer(Context& context, Buffer& buffer);
void copyBuffer(Context& context, Buffer& srcBuffer, Buffer& dstBuffer);
void copyBufferToImage(Context& context, Buffer& buffer, Image& image);

void createImage(Context& context, Image& image);
void destroyImage(Context& context, Image& image);
void createImageView(Context& context, Image& image);
void transitionImageLayout(Context& context, Image& image, VkImageLayout oldLayout, VkImageLayout newLayout);

void createSampler(Context& context, Sampler& sampler);

u32 findMemoryType(Context& context, VkMemoryRequirements const& requirements, VkMemoryPropertyFlags properties);

VkCommandBuffer beginCommandBufferOneTimeSubmit(Context& context);
void endCommandBufferOneTimeSubmit(Context& context, VkCommandBuffer commandBuffer);
