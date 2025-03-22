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
    THROW_IF_FAILED(vkCreateShaderModule(context.device.device, &createInfo, context.allocator, &module), __FILE__, __LINE__, "Failed to create shader module");

    return module;
}
