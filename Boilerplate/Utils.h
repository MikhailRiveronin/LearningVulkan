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

u32 findMemoryType(Context& context, VkMemoryRequirements const& requirements, VkMemoryPropertyFlags properties);
