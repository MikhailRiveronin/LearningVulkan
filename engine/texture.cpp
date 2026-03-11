#include "texture.h"

#include "vulkan_struct_initializers.h"

Texture::Texture(Texture_Manager* creator, VkFormat format, VkExtent3D extent, VkImageUsageFlags usage) : Resource(creator), format(format)
{
    auto image_create_info = Vulkan_Struct_Initializers::image_create_info(format, extent, usage);
    VmaAllocationCreateInfo allocation_create_info = {};
    allocation_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    VK_CHECK(vmaCreateImage(creator->device->allocator, &image_create_info, &allocation_create_info, &handle, &allocation, 0));

    auto image_view_create_info = Vulkan_Struct_Initializers::image_view_create_info(handle, format);
    VK_CHECK(vkCreateImageView(creator->device->handle, &image_view_create_info, nullptr, &image_view));

    auto sampler_create_info = Vulkan_Struct_Initializers::sampler_create_info();
    VK_CHECK(vkCreateSampler(creator->device->handle, &sampler_create_info, nullptr, &sampler));

}
