#pragma once

#include "Defines.h"
#include "Logger.h"
#include "Structures.h"

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
VkShaderModule createShaderModule(Globals& globals, std::vector<u8> const& code);

void createBuffer(Globals& globals, Buffer& buffer);
void destroyBuffer(Globals& globals, Buffer& buffer);
void copyBuffer(Globals& globals, Buffer& srcBuffer, Buffer& dstBuffer);
void copyBufferToImage(Globals& globals, Buffer& buffer, Image& image);

void createImage(Globals& globals, Image& image);
void destroyImage(Globals const& globals, Image const& image);
void createImageView(Globals& globals, Image& image);
void transitionImageLayout(Globals& globals, Image& image, VkImageLayout oldLayout, VkImageLayout newLayout);

void createSampler(Globals& globals, Sampler& sampler);

u32 findMemoryType(Globals& globals, VkMemoryRequirements const& requirements, VkMemoryPropertyFlags properties);

VkCommandBuffer beginCommandBufferOneTimeSubmit(Globals& globals);
void endCommandBufferOneTimeSubmit(Globals& globals, VkCommandBuffer commandBuffer);
