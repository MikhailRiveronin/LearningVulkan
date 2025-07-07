#include "Utils.h"
#include "Initializer.h"

#include <stb_image.h>
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

void createBuffer(Context const& context, Buffer& buffer)
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
    THROW_IF_FAILED(vkCreateBuffer(context.device.handle, &createInfo, context.allocator, &buffer.handle));

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(context.device.handle, buffer.handle, &memoryRequirements);
    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryTypeIndex(context, memoryRequirements, buffer.memoryProperties);
    THROW_IF_FAILED(vkAllocateMemory(context.device.handle, &allocateInfo, context.allocator, &buffer.memory));
    THROW_IF_FAILED(vkBindBufferMemory(context.device.handle, buffer.handle, buffer.memory, 0));
}

void createImage(Context const& context, Image& image)
{
    {
        VkImageCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = image.flags;
        createInfo.imageType = image.imageType;
        createInfo.format = image.format;
        createInfo.extent = { image.width, image.height, 1 };
        createInfo.mipLevels = image.mipLevels;
        createInfo.arrayLayers = image.arrayLayers;
        createInfo.samples = image.samples;
        createInfo.tiling = image.tiling;
        createInfo.usage = image.usage;
        createInfo.sharingMode = image.sharingMode;
        createInfo.queueFamilyIndexCount = image.queueFamilyIndexCount;
        createInfo.pQueueFamilyIndices = image.queueFamilyIndices;
        createInfo.initialLayout = image.initialLayout;
        THROW_IF_FAILED(vkCreateImage(context.device.handle, &createInfo, context.allocator, &image.handle));
    
        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(context.device.handle, image.handle, &memoryRequirements);
        VkMemoryAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.pNext = nullptr;
        allocateInfo.allocationSize = memoryRequirements.size;
        allocateInfo.memoryTypeIndex = findMemoryTypeIndex(context, memoryRequirements, image.memoryProperties);
        THROW_IF_FAILED(vkAllocateMemory(context.device.handle, &allocateInfo, context.allocator, &image.memory));
        THROW_IF_FAILED(vkBindImageMemory(context.device.handle, image.handle, image.memory, 0));
    }
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.image = image.handle;
        createInfo.viewType = image.view.viewType;
        createInfo.format = image.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = image.view.aspectMask;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = image.mipLevels;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = image.arrayLayers;
        THROW_IF_FAILED(vkCreateImageView(context.device.handle, &createInfo, context.allocator, &image.view.handle));
    }
    {
        VkSamplerCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.magFilter = image.sampler.magFilter;
        createInfo.minFilter = image.sampler.minFilter;
        createInfo.mipmapMode = image.sampler.mipmapMode;
        createInfo.addressModeU = image.sampler.addressModeU;
        createInfo.addressModeV = image.sampler.addressModeV;
        createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        createInfo.mipLodBias = 0.f;
        createInfo.anisotropyEnable = VK_TRUE;
        createInfo.maxAnisotropy = 1.f;
        createInfo.compareEnable = VK_FALSE;
        createInfo.compareOp = VK_COMPARE_OP_NEVER;
        createInfo.minLod = 0.f;
        createInfo.maxLod = 0.f;
        createInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        createInfo.unnormalizedCoordinates = VK_FALSE;
        THROW_IF_FAILED(vkCreateSampler(context.device.handle, &createInfo, context.allocator, &image.sampler.handle));
    }
}

void destroyBuffer(Context const& context, Buffer& buffer)
{
    vkDestroyBuffer(context.device.handle, buffer.handle, context.allocator);
    vkFreeMemory(context.device.handle, buffer.memory, context.allocator);
}

void destroyImage(Context const& context, Image const& image)
{
    vkDestroyImage(context.device.handle, image.handle, context.allocator);
    vkFreeMemory(context.device.handle, image.memory, context.allocator);
}

void copyBuffer(Context const& globals, Buffer& srcBuffer, Buffer& dstBuffer)
{
    auto commandBuffer = beginCommandBufferOneTimeSubmit(globals);

    VkBufferCopy bufferCopy = {};
    bufferCopy.srcOffset = 0;
    bufferCopy.dstOffset = 0;
    bufferCopy.size = srcBuffer.size;

    vkCmdCopyBuffer(commandBuffer, srcBuffer.handle, dstBuffer.handle, 1, &bufferCopy);

    endCommandBufferOneTimeSubmit(globals, commandBuffer);
}

void copyBufferToImage(Context const& context, Buffer& buffer, Image& image)
{
    VkBufferImageCopy copy = {};
    copy.bufferOffset = 0;
    copy.bufferRowLength = 0;
    copy.bufferImageHeight = 0;
    copy.imageSubresource.aspectMask = image.view.aspectMask;
    copy.imageSubresource.mipLevel = 0;
    copy.imageSubresource.baseArrayLayer = 0;
    copy.imageSubresource.layerCount = image.arrayLayers;
    copy.imageOffset = { 0, 0, 0 };
    copy.imageExtent = { image.width, image.height, 1};

    auto commandBuffer = beginCommandBufferOneTimeSubmit(context);
    vkCmdCopyBufferToImage(commandBuffer, buffer.handle, image.handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    endCommandBufferOneTimeSubmit(context, commandBuffer);
}

VkDeviceSize calculateUniformBufferAlignment(Context const& globals, VkDeviceSize size)
{
    VkDeviceSize minAlignment = globals.device.support.properties.limits.minUniformBufferOffsetAlignment;
    return (size + minAlignment - 1) & ~(minAlignment - 1);
}



void createImageView(Context const& globals, Image& image)
{
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.image = image.handle;
    createInfo.viewType = image.view.viewType;
    createInfo.format = image.format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = image.view.aspectMask;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = image.mipLevels;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = image.arrayLayers;
    THROW_IF_FAILED(vkCreateImageView(globals.device.handle, &createInfo, globals.allocator, &image.view.handle));
}

void transitionImageLayout(Context const& globals, Image& image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
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
    barrier.subresourceRange.aspectMask = image.view.aspectMask;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = image.mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = image.arrayLayers;

    auto commandBuffer = beginCommandBufferOneTimeSubmit(globals);
    vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    endCommandBufferOneTimeSubmit(globals, commandBuffer);
}

u32 findMemoryTypeIndex(Context const& globals, VkMemoryRequirements const& requirements, VkMemoryPropertyFlags properties)
{
    for (uint32_t i = 0; i < globals.device.support.memoryProperties.memoryTypeCount; ++i) {
        if ((requirements.memoryTypeBits & (1 << i)) && ((globals.device.support.memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)) {
            return i;
        }
    }
    LOG_ERROR("Failed to find suitable memory type");
    return -1;
}

VkCommandBuffer beginCommandBufferOneTimeSubmit(Context const& globals)
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

void endCommandBufferOneTimeSubmit(Context const& globals, VkCommandBuffer commandBuffer)
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

void createDescriptorSets(Context const& globals, std::vector<DescriptorSetBinding> const& descriptorSetBindings, DescriptorSets& descriptorSets)
{
    std::vector<VkDescriptorPoolSize> poolSizes(descriptorSetBindings.size());
    std::vector<VkDescriptorSetLayoutBinding> bindings(descriptorSetBindings.size());
    for (u32 i = 0; i < descriptorSetBindings.size(); ++i) {
        poolSizes[i] = Initializer::descriptorPoolSize(descriptorSetBindings[i].binding.descriptorType, globals.swapchain.images.size());
        bindings[i] = descriptorSetBindings[i].binding;
    }
    auto descriptorPoolCreateInfo = Initializer::descriptorPoolCreateInfo(globals.swapchain.images.size(), poolSizes);
    THROW_IF_FAILED(
        vkCreateDescriptorPool(globals.device.handle, &descriptorPoolCreateInfo, globals.allocator, &descriptorSets.pool),
        __FILE__, __LINE__,
        "Failed to create descriptor pool");

    auto descriptorSetLayoutCreateInfo = Initializer::descriptorSetLayoutCreateInfo(bindings);
    THROW_IF_FAILED(
        vkCreateDescriptorSetLayout(globals.device.handle, &descriptorSetLayoutCreateInfo, globals.allocator, &descriptorSets.setLayout),
        __FILE__, __LINE__,
        "Failed to create descriptor set layout");
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts(globals.swapchain.images.size(), descriptorSets.setLayout);

    descriptorSets.handles.resize(globals.swapchain.images.size());
    auto descriptorSetAllocateInfo = Initializer::descriptorSetAllocateInfo(descriptorSets.pool, globals.swapchain.images.size(), descriptorSetLayouts);
    THROW_IF_FAILED(
        vkAllocateDescriptorSets(globals.device.handle, &descriptorSetAllocateInfo, descriptorSets.handles.data()),
        __FILE__, __LINE__,
        "Failed to allocate descriptor sets");

    for (u32 i = 0; i < globals.swapchain.images.size(); ++i) {
        std::vector<VkWriteDescriptorSet> descriptorWrites(descriptorSetBindings.size());
        for (u32 j = 0; j < descriptorWrites.size(); ++j) {
            descriptorWrites[j] = descriptorSetBindings[j].descriptorWrite;
            descriptorWrites[j].dstSet = descriptorSets.handles[i];
        }
        vkUpdateDescriptorSets(globals.device.handle, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }
}

void destroyDescriptorSets(Context const& globals, DescriptorSets& descriptorSets)
{
    vkDestroyDescriptorSetLayout(globals.device.handle, descriptorSets.setLayout, globals.allocator);
    vkDestroyDescriptorPool(globals.device.handle, descriptorSets.pool, globals.allocator);
}

void createPipeline(Context const& globals,  Pipeline& pipeline)
{
    std::vector<VkPipelineShaderStageCreateInfo> stages(2);
    VkShaderModule shaderModule[2];
    {
        auto code = loadShaderCode(pipeline.vsFilename);
        auto shaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(code);
        THROW_IF_FAILED(
            vkCreateShaderModule(globals.device.handle, &shaderModuleCreateInfo, globals.allocator, &shaderModule[0]),
            __FILE__, __LINE__,
            "Failed to create shader module");
        stages[0] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, shaderModule[0]);
    }
    {
        auto code = loadShaderCode(pipeline.fsFilename);
        auto shaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(code);
        THROW_IF_FAILED(
            vkCreateShaderModule(globals.device.handle, &shaderModuleCreateInfo, globals.allocator, &shaderModule[1]),
            __FILE__, __LINE__,
            "Failed to create shader module");
        stages[1] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, shaderModule[1]);
    }

    auto vertexInputState = Initializer::pipelineVertexInputStateCreateInfo(pipeline.vertexBindingDescriptions, pipeline.vertexAttributeDescriptions);
    auto inputAssemblyState = Initializer::pipelineInputAssemblyStateCreateInfo();
    auto tessellationState = Initializer::pipelineTessellationStateCreateInfo();

    std::vector<VkViewport> viewports(1);
    viewports[0] = Initializer::viewport(globals.swapchain.extent.width, globals.swapchain.extent.height);
    std::vector<VkRect2D> scissors(1);
    scissors[0] = Initializer::scissor(globals.swapchain.extent.width, globals.swapchain.extent.height);
    auto viewportState = Initializer::pipelineViewportStateCreateInfo(viewports, scissors);

    auto rasterizationState = Initializer::pipelineRasterizationStateCreateInfo();
    auto multisampleState = Initializer::pipelineMultisampleStateCreateInfo();
    auto depthStencilState = Initializer::pipelineDepthStencilStateCreateInfo();

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates(1);
    colorBlendAttachmentStates[0] = Initializer::pipelineColorBlendAttachmentState();
    auto colorBlendState = Initializer::pipelineColorBlendStateCreateInfo(colorBlendAttachmentStates);

    std::vector<VkDynamicState> dynamicStates(2);
    dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
    auto dynamicState = Initializer::pipelineDynamicStateCreateInfo(dynamicStates);

    auto pipelineLayoutCreateInfo = Initializer::pipelineLayoutCreateInfo(pipeline.descriptorSetLayouts, pipeline.pushConstantRanges);
    THROW_IF_FAILED(
        vkCreatePipelineLayout(globals.device.handle, &pipelineLayoutCreateInfo, globals.allocator, &pipeline.layout),
        __FILE__, __LINE__,
        "Failed to create pipeline layout");

    auto createInfo = Initializer::graphicsPipelineCreateInfo(
        stages,
        &vertexInputState,
        &inputAssemblyState,
        &tessellationState,
        &viewportState,
        &rasterizationState,
        &multisampleState,
        &depthStencilState,
        &colorBlendState,
        &dynamicState,
        pipeline.layout,
        globals.renderPass);
    THROW_IF_FAILED(
        vkCreateGraphicsPipelines(globals.device.handle, VK_NULL_HANDLE, 1, &createInfo, globals.allocator, &pipeline.handle),
        __FILE__, __LINE__,
        "Failed to create graphics pipeline");

    vkDestroyShaderModule(globals.device.handle, shaderModule[0], globals.allocator);
    vkDestroyShaderModule(globals.device.handle, shaderModule[1], globals.allocator);
}

void destroyPipeline(Context const& globals, Pipeline& pipeline)
{
    vkDestroyPipelineLayout(globals.device.handle, pipeline.layout, globals.allocator);
    vkDestroyPipeline(globals.device.handle, pipeline.handle, globals.allocator);
}

u32 calculateAlignedSize(u32 size, u32 alignment)
{
    if (alignment == 0) {
        throw std::runtime_error("Alignment cannot be zero");
    }
    return (size + alignment - 1) / alignment * alignment;
}

void createShaderStage(Context const& globals, ShaderStage shaderStage)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    std::string shaderFolder = "D:/Projects/LearningVulkan/build/Shaders/";
    std::string path = shaderFolder + shaderStage.filename;
    std::ifstream file(path, std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    auto fileSize = (u32)file.tellg();
    std::vector<char> source(fileSize);
    file.seekg(0);
    file.read(source.data(), fileSize);
    file.close();

    shaderc_shader_kind kind;
    switch (shaderStage.stage) {
    case VK_SHADER_STAGE_VERTEX_BIT:
        kind = shaderc_glsl_vertex_shader;
        break;

    case VK_SHADER_STAGE_FRAGMENT_BIT:
        kind = shaderc_glsl_vertex_shader;
        break;

    default:
        kind = shaderc_glsl_vertex_shader;
    }

    auto code = compiler.CompileGlslToSpv(source.data(), kind, shaderStage.filename.c_str());
    auto shaderModuleCreateInfo = Initializer::shaderModuleCreateInfo({ code.begin(), code.end() });
    THROW_IF_FAILED(
        vkCreateShaderModule(globals.device.handle, &shaderModuleCreateInfo, globals.allocator, &shaderStage.module),
        __FILE__, __LINE__,
        "Failed to create shader module");
    shaderStage.createInfo = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, shaderStage.module);
}

void createCubeTexture(
    Context const& context,
    char const* front, char const* back,
    char const* left, char const* right,
    char const* up, char const* down,
    Image& image)
{
    i32 width;
    i32 height;
    i32 channels;
    uc* pixels[6];
    pixels[0] = stbi_load(front, &width, &height, &channels, STBI_rgb_alpha);
    if (!pixels[0]) {
        throw std::runtime_error("Failed to load texture image");
    }

    pixels[1] = stbi_load(back, &width, &height, &channels, STBI_rgb_alpha);
    if (!pixels[1]) {
        throw std::runtime_error("Failed to load texture image");
    }

    pixels[2] = stbi_load(left, &width, &height, &channels, STBI_rgb_alpha);
    if (!pixels[2]) {
        throw std::runtime_error("Failed to load texture image");
    }

    pixels[3] = stbi_load(right, &width, &height, &channels, STBI_rgb_alpha);
    if (!pixels[3]) {
        throw std::runtime_error("Failed to load texture image");
    }

    pixels[4] = stbi_load(up, &width, &height, &channels, STBI_rgb_alpha);
    if (!pixels[4]) {
        throw std::runtime_error("Failed to load texture image");
    }

    pixels[5] = stbi_load(down, &width, &height, &channels, STBI_rgb_alpha);
    if (!pixels[5]) {
        throw std::runtime_error("Failed to load texture image");
    }

    u32 layerSize = width * height * STBI_rgb_alpha;
    u32 imageSize = layerSize * 6;

    Buffer stagingBuffer;
    stagingBuffer.size = imageSize;
    stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBuffer.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    createBuffer(context, stagingBuffer);
    vkMapMemory(context.device.handle, stagingBuffer.memory, 0, stagingBuffer.size, 0, &stagingBuffer.mapped);
    for (u32 i = 0; i < 6; ++i) {
        memcpy(static_cast<u8*>(stagingBuffer.mapped) + layerSize * i, pixels[i], layerSize);
    }

    vkUnmapMemory(context.device.handle, stagingBuffer.memory);
    stbi_image_free(pixels);

    image.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    image.width = width;
    image.height = height;
    image.arrayLayers = 6;
    image.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image.view.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    createImage(context, image);
    transitionImageLayout(context, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(context, stagingBuffer, image);
    destroyBuffer(context, stagingBuffer);
    transitionImageLayout(context, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}
