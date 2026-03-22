#pragma once

#include "defines.h"

#include "resource.h"
#include "texture_manager.h"
#include "vk_mem_alloc.h"

class Texture : public Resource
{
public:
    std::string name;
    u32 width;
    u32 height;
    u32 channel_count;
    std::vector<unsigned char> data;




    // VkImage handle;
    // VkImageView image_view;
    // VkSampler sampler;
    // VmaAllocation allocation;

    // Texture(Texture_Manager* creator, VkFormat format, VkExtent3D extent, VkImageUsageFlags usage);







    // VkDeviceMemory memory = VK_NULL_HANDLE;
    // VkImageCreateFlags flags = 0;
    // VkImageType imageType = VK_IMAGE_TYPE_2D;
    // VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    // u32 width = 0;
    // u32 height = 0;
    // u32 mipLevels = 1;
    // u32 arrayLayers = 1;
    // VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    // VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    // VkImageUsageFlags usage = 0;
    // VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // u32 queueFamilyIndexCount = 0;
    // u32* queueFamilyIndices = nullptr;
    // VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // VkMemoryPropertyFlags memoryProperties = 0;

    // struct {
    //     VkImageView handle = VK_NULL_HANDLE;
    //     VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
    //     VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    // } view;

    // struct {
    //     VkSampler handle = VK_NULL_HANDLE;
    //     VkFilter magFilter = VK_FILTER_LINEAR;
    //     VkFilter minFilter = VK_FILTER_LINEAR;
    //     VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    //     VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    //     VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    // } sampler;

    VkAttachmentLoadOp loadOp;


private:
    VkFormat format;

};
