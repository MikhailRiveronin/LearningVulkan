#pragma once

#include <vulkan/vulkan.h>

struct Globals
{
    VkDevice device = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer;
};

struct Buffer
{
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    VkDescriptorBufferInfo descriptorBufferInfo = {};
    void* mappedData = nullptr;
};

struct ImageInfo
{
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkDescriptorImageInfo descriptorInfo = VK_NULL_HANDLE;

    VkFormat format;
    uint32_t width;
    uint32_t height;
    uint32_t mipLevels;
    VkImageUsageFlags usageFlags;
    VkMemoryPropertyFlags memoryPropertyFlags;

    VkImageLayout layout;
};

struct SamplerInfo
{
    VkSampler sampler = VK_NULL_HANDLE;

    VkFilter magFilter;
    VkFilter minFilter;
    VkSamplerMipmapMode mipmapMode;
    VkSamplerAddressMode addressModeU;
    VkSamplerAddressMode addressModeV;
};
