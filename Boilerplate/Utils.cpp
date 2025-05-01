#include "Utils.h"

#include <fstream>

std::vector<char> loadShaderCode(std::string const& filename)
{
    std::string shaderFolder = "D:/Projects/LearningVulkan/build/Shaders/";
    std::string path = shaderFolder + filename;
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    auto fileSize = (u32)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

void createBuffer(Globals const& globals, Buffer& buffer)
{
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.size = buffer.size;
    createInfo.usage = buffer.usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
    THROW_IF_FAILED(
        vkCreateBuffer(globals.device.handle, &createInfo, globals.allocator, &buffer.handle),
        __FILE__, __LINE__,
        "Failed to create buffer");

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(globals.device.handle, buffer.handle, &memoryRequirements);
    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(globals, memoryRequirements, buffer.memoryProperties);
    THROW_IF_FAILED(
        vkAllocateMemory(globals.device.handle, &allocateInfo, globals.allocator, &buffer.memory),
        __FILE__, __LINE__,
        "Failed to allocate memory");

    THROW_IF_FAILED(
        vkBindBufferMemory(globals.device.handle, buffer.handle, buffer.memory, 0),
        __FILE__, __LINE__,
        "Failed to bind buffer memory");
}

void destroyBuffer(Globals const& globals, Buffer& buffer)
{
    vkDestroyBuffer(globals.device.handle, buffer.handle, globals.allocator);
    vkFreeMemory(globals.device.handle, buffer.memory, globals.allocator);
}

void copyBuffer(Globals const& globals, Buffer& srcBuffer, Buffer& dstBuffer)
{
    auto commandBuffer = beginCommandBufferOneTimeSubmit(globals);

    VkBufferCopy bufferCopy = {};
    bufferCopy.srcOffset = 0;
    bufferCopy.dstOffset = 0;
    bufferCopy.size = srcBuffer.size;

    vkCmdCopyBuffer(commandBuffer, srcBuffer.handle, dstBuffer.handle, 1, &bufferCopy);

    endCommandBufferOneTimeSubmit(globals, commandBuffer);
}

void copyBufferToImage(Globals& globals, Buffer& buffer, Image& image)
{
    auto commandBuffer = beginCommandBufferOneTimeSubmit(globals);

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

    endCommandBufferOneTimeSubmit(globals, commandBuffer);
}

VkDeviceSize calculateUniformBufferAlignment(Globals const& globals, VkDeviceSize size)
{
    VkDeviceSize minAlignment = globals.device.support.properties.limits.minUniformBufferOffsetAlignment;
    return (size + minAlignment - 1) & ~(minAlignment - 1);
}

void createImage(Globals& globals, Image& image)
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

    THROW_IF_FAILED(vkCreateImage(globals.device.handle, &createInfo, globals.allocator, &image.handle), __FILE__, __LINE__, "Failed to create image");

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(globals.device.handle, image.handle, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(globals, memoryRequirements, image.memoryProperties);

    THROW_IF_FAILED(vkAllocateMemory(globals.device.handle, &allocateInfo, globals.allocator, &image.memory), __FILE__, __LINE__, "Failed to allocate memory");
    THROW_IF_FAILED(vkBindImageMemory(globals.device.handle, image.handle, image.memory, 0), __FILE__, __LINE__, "Failed to bind image memory");
}

void destroyImage(Globals const& globals, Image const& image)
{
    vkDestroyImage(globals.device.handle, image.handle, globals.allocator);
    vkFreeMemory(globals.device.handle, image.memory, globals.allocator);
}

void createImageView(Globals& globals, Image& image)
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

    THROW_IF_FAILED(vkCreateImageView(globals.device.handle, &createInfo, globals.allocator, &image.view), __FILE__, __LINE__, "Failed to create image view");
}

void transitionImageLayout(Globals& globals, Image& image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    auto commandBuffer = beginCommandBufferOneTimeSubmit(globals);

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


    endCommandBufferOneTimeSubmit(globals, commandBuffer);
}

void createSampler(Globals& globals, Sampler& sampler)
{
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
    createInfo.maxAnisotropy = globals.device.support.properties.limits.maxSamplerAnisotropy;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_NEVER;
    createInfo.minLod = 0.f;
    createInfo.maxLod = 0.f;
    createInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    createInfo.unnormalizedCoordinates = VK_FALSE;

    THROW_IF_FAILED(vkCreateSampler(globals.device.handle, &createInfo, globals.allocator, &sampler.handle), __FILE__, __LINE__, "Failed to create sampler");
}

u32 findMemoryType(Globals const& globals, VkMemoryRequirements const& requirements, VkMemoryPropertyFlags properties)
{
    for (uint32_t i = 0; i < globals.device.support.memoryProperties.memoryTypeCount; ++i) {
        if ((requirements.memoryTypeBits & (1 << i)) && ((globals.device.support.memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)) {
            return i;
        }
    }
    LOG_ERROR("Failed to find suitable memory type");
    return -1;
}

VkCommandBuffer beginCommandBufferOneTimeSubmit(Globals const& globals)
{
    VkCommandBuffer commandBuffer;

    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = globals.graphicsCommandBuffer.pool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    THROW_IF_FAILED(vkAllocateCommandBuffers(globals.device.handle, &allocateInfo, &commandBuffer), __FILE__, __LINE__, "Failed to allocate command buffers");

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;

    THROW_IF_FAILED(vkBeginCommandBuffer(commandBuffer, &beginInfo), __FILE__, __LINE__, "Failed to begin command buffer");

    return commandBuffer;
}

void endCommandBufferOneTimeSubmit(Globals const& globals, VkCommandBuffer commandBuffer)
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
    THROW_IF_FAILED(
        vkQueueSubmit(globals.device.queues.graphics.handle, 1, &submitInfo, VK_NULL_HANDLE),
        __FILE__, __LINE__,
        "Failed to submit command buffers");
    THROW_IF_FAILED(
        vkQueueWaitIdle(globals.device.queues.graphics.handle),
        __FILE__, __LINE__,
        "Failed to wait for a queue to become idle");

    vkFreeCommandBuffers(globals.device.handle, globals.graphicsCommandBuffer.pool, 1, &commandBuffer);
}

u32 calculateAlignedSize(u32 size, u32 alignment)
{
    if (alignment == 0) {
        throw std::runtime_error("Alignment cannot be zero");
    }
    return (size + alignment - 1) / alignment * alignment;
}
