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

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(context.device.handle, buffer.handle, &requirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = requirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(context, requirements, buffer.memoryProperties);

    THROW_IF_FAILED(vkAllocateMemory(context.device.handle, &allocateInfo, context.allocator, &buffer.memory), __FILE__, __LINE__, "Failed to allocate memory");

    THROW_IF_FAILED(vkBindBufferMemory(context.device.handle, buffer.handle, buffer.memory, buffer.memoryOffset), __FILE__, __LINE__, "Failed to bind buffer memory");
}

void destroyBuffer(Context& context, Buffer& buffer)
{
    vkDestroyBuffer(context.device.handle, buffer.handle, context.allocator);
    vkFreeMemory(context.device.handle, buffer.memory, context.allocator);
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
