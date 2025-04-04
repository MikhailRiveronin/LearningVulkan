#include "Utils.h"

#include <fstream>

std::vector<u8> loadShaderBinary(std::string const& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    auto fileSize = file.tellg();
    std::vector<u8> buffer(fileSize);
    file.seekg(0);
    file.read((char*)buffer.data(), fileSize);
    file.close();

    return buffer;
}

VkShaderModule createShaderModule(Context& context, std::vector<u8> const& code)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.codeSize = code.size();
    createInfo.pCode = (const u32*)code.data();

    VkShaderModule module;
    THROW_IF_FAILED(vkCreateShaderModule(context.device.handle, &createInfo, context.allocator, &module), __FILE__, __LINE__, "Failed to create shader module");

    return module;
}

void createBuffer(Context& context, Buffer& buffer)
{
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.size = buffer.size;
    createInfo.usage = buffer.usage;
    createInfo.sharingMode = buffer.sharingMode;
    createInfo.queueFamilyIndexCount = buffer.queueFamilyIndexCount;
    createInfo.pQueueFamilyIndices = buffer.queueFamilyIndices;

    THROW_IF_FAILED(vkCreateBuffer(context.device.handle, &createInfo, context.allocator, &buffer.handle), __FILE__, __LINE__, "Failed to create buffer");

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(context.device.handle, buffer.handle, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(context, memoryRequirements, buffer.memoryProperties);

    THROW_IF_FAILED(vkAllocateMemory(context.device.handle, &allocateInfo, context.allocator, &buffer.memory), __FILE__, __LINE__, "Failed to allocate memory");
    THROW_IF_FAILED(vkBindBufferMemory(context.device.handle, buffer.handle, buffer.memory, buffer.memoryOffset), __FILE__, __LINE__, "Failed to bind buffer memory");
}

void destroyBuffer(Context& context, Buffer& buffer)
{
    vkDestroyBuffer(context.device.handle, buffer.handle, context.allocator);
    vkFreeMemory(context.device.handle, buffer.memory, context.allocator);
}

void copyBuffer(Context& context, Buffer& srcBuffer, Buffer& dstBuffer)
{
    auto commandBuffer = beginCommandBufferOneTimeSubmit(context);

    VkBufferCopy bufferCopy = {};
    bufferCopy.srcOffset = 0;
    bufferCopy.dstOffset = 0;
    bufferCopy.size = srcBuffer.size;

    vkCmdCopyBuffer(commandBuffer, srcBuffer.handle, dstBuffer.handle, 1, &bufferCopy);

    endCommandBufferOneTimeSubmit(context, commandBuffer);
}

void copyBufferToImage(Context& context, Buffer& buffer, Image& image)
{
    auto commandBuffer = beginCommandBufferOneTimeSubmit(context);

    VkBufferImageCopy copy = {};
    copy.bufferOffset = 0;
    copy.bufferRowLength = 0;
    copy.bufferImageHeight = 0;
    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.mipLevel = 0;
    copy.imageSubresource.baseArrayLayer = 0;
    copy.imageSubresource.layerCount = 1;
    copy.imageOffset = { 0, 0, 0 };
    copy.imageExtent = { image.width, image.height, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer.handle, image.handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

    endCommandBufferOneTimeSubmit(context, commandBuffer);
}

void createImage(Context& context, Image& image)
{
    VkImageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.format = image.format;
    createInfo.extent = { image.width, image.height, 1 };
    createInfo.mipLevels = image.mipLevels;
    createInfo.arrayLayers = image.arrayLayers;
    createInfo.samples = image.samples;
    createInfo.tiling = image.tiling;
    createInfo.usage = image.usage;
    createInfo.sharingMode = image.sharingMode;
    createInfo.queueFamilyIndexCount = image.queueCount;
    createInfo.pQueueFamilyIndices = image.queueIndices;
    createInfo.initialLayout = image.layout;

    THROW_IF_FAILED(vkCreateImage(context.device.handle, &createInfo, context.allocator, &image.handle), __FILE__, __LINE__, "Failed to create image");

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(context.device.handle, image.handle, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(context, memoryRequirements, image.memoryProperties);

    THROW_IF_FAILED(vkAllocateMemory(context.device.handle, &allocateInfo, context.allocator, &image.memory), __FILE__, __LINE__, "Failed to allocate memory");
    THROW_IF_FAILED(vkBindImageMemory(context.device.handle, image.handle, image.memory, 0), __FILE__, __LINE__, "Failed to bind image memory");
}

void destroyImage(Context& context, Image& image)
{
    vkDestroyImage(context.device.handle, image.handle, context.allocator);
    vkFreeMemory(context.device.handle, image.memory, context.allocator);
}

void createImageView(Context& context, Image& image)
{
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.image = image.handle;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = image.format;
    createInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
    createInfo.subresourceRange.aspectMask = image.aspect;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = image.mipLevels;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = image.arrayLayers;

    THROW_IF_FAILED(vkCreateImageView(context.device.handle, &createInfo, context.allocator, &image.view), __FILE__, __LINE__, "Failed to create image view");
}

void transitionImageLayout(Context& context, Image& image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    auto commandBuffer = beginCommandBufferOneTimeSubmit(context);

    VkAccessFlags srcAccessMask;
    VkAccessFlags dstAccessMask;
    VkPipelineStageFlags srcStageMask;
    VkPipelineStageFlags dstStageMask;
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        srcAccessMask = 0;
        dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::runtime_error("Unsupported layout transition");
    }

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext = nullptr;
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstAccessMask = dstAccessMask;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image.handle;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);


    endCommandBufferOneTimeSubmit(context, commandBuffer);
}

void createSampler(Context& context, Sampler& sampler)
{
    VkPhysicalDeviceProperties physicalDeviceProperties = {};
    vkGetPhysicalDeviceProperties(context.device.physicalDevice, &physicalDeviceProperties);

    VkSamplerCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.magFilter = sampler.magFilter;
    createInfo.minFilter = sampler.minFilter;
    createInfo.mipmapMode = sampler.mipmapMode;
    createInfo.addressModeU = sampler.addressModeU;
    createInfo.addressModeV = sampler.addressModeV;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.mipLodBias = 0.f;
    createInfo.anisotropyEnable = VK_TRUE;
    createInfo.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_NEVER;
    createInfo.minLod = 0.f;
    createInfo.maxLod = 0.f;
    createInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    createInfo.unnormalizedCoordinates = VK_FALSE;

    THROW_IF_FAILED(vkCreateSampler(context.device.handle, &createInfo, context.allocator, &sampler.handle), __FILE__, __LINE__, "Failed to create sampler");
}

u32 findMemoryType(Context& context, VkMemoryRequirements const& requirements, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(context.device.physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        if ((requirements.memoryTypeBits & (1 << i)) && ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)) {
            return i;
        }
    }

    LOG_ERROR("Failed to find suitable memory type");

    return -1;
}

VkCommandBuffer beginCommandBufferOneTimeSubmit(Context& context)
{
    VkCommandBuffer commandBuffer;

    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = context.commandBuffer.pool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    THROW_IF_FAILED(vkAllocateCommandBuffers(context.device.handle, &allocateInfo, &commandBuffer), __FILE__, __LINE__, "Failed to allocate command buffers");

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;

    THROW_IF_FAILED(vkBeginCommandBuffer(commandBuffer, &beginInfo), __FILE__, __LINE__, "Failed to begin command buffer");

    return commandBuffer;
}

void endCommandBufferOneTimeSubmit(Context& context, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    THROW_IF_FAILED(vkQueueSubmit(context.device.queues.graphics, 1, &submitInfo, VK_NULL_HANDLE), __FILE__, __LINE__, "Failed to submit command buffers");
    THROW_IF_FAILED(vkQueueWaitIdle(context.device.queues.graphics), __FILE__, __LINE__, "Failed to wait for a queue to become idle");

    vkFreeCommandBuffers(context.device.handle, context.commandBuffer.pool, 1, &commandBuffer);
}
