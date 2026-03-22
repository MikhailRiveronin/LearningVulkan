#include "Utils.h"
#include "vulkan_struct_initializers.h"
#include "scene_manager.h"

#include "mesh.h"
#include "mesh_manager.h"
#include "texture.h"
#include "texture_manager.h"

#include "third_party/tiny_gltf.h"

#include "third_party/stb_image.h"
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
    VK_CHECK(vkCreateBuffer(context.device.handle, &createInfo, context.allocator, &buffer.handle));

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(context.device.handle, buffer.handle, &memoryRequirements);
    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryTypeIndex(context, memoryRequirements, buffer.memoryProperties);
    VK_CHECK(vkAllocateMemory(context.device.handle, &allocateInfo, context.allocator, &buffer.memory));
    VK_CHECK(vkBindBufferMemory(context.device.handle, buffer.handle, buffer.memory, 0));
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
        VK_CHECK(vkCreateImage(context.device.handle, &createInfo, context.allocator, &image.handle));
    
        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(context.device.handle, image.handle, &memoryRequirements);
        VkMemoryAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.pNext = nullptr;
        allocateInfo.allocationSize = memoryRequirements.size;
        allocateInfo.memoryTypeIndex = findMemoryTypeIndex(context, memoryRequirements, image.memoryProperties);
        VK_CHECK(vkAllocateMemory(context.device.handle, &allocateInfo, context.allocator, &image.memory));
        VK_CHECK(vkBindImageMemory(context.device.handle, image.handle, image.memory, 0));
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
        VK_CHECK(vkCreateImageView(context.device.handle, &createInfo, context.allocator, &image.view.handle));
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
        VK_CHECK(vkCreateSampler(context.device.handle, &createInfo, context.allocator, &image.sampler.handle));
    }
}

void create_attachment(Context const& context, Attachment& attachment)
{
    auto image_create_info = Vulkan_Struct_Initializers::image_create_info(attachment.format, attachment.width, attachment.height, attachment.usage);
    VmaAllocationCreateInfo image_allocation_create_info = {};
    image_allocation_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    VK_CHECK(vmaCreateImage(context.device->allocator, &image_create_info, &image_allocation_create_info, &attachment.handle, &attachment.allocation, 0));

    auto image_view_create_info = Vulkan_Struct_Initializers::image_view_create_info(attachment.handle, VK_FORMAT_D24_UNORM_S8_UINT);
    VK_CHECK(vkCreateImageView(context.device->handle, &image_view_create_info, nullptr, &attachment.view.handle));
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

void destroy_attachment(Context const& context, Attachment& attachment)
{
    vmaDestroyImage(context.device->allocator, attachment.handle, attachment.allocation);
    vmaFreeMemory(context.device->allocator, attachment.allocation);
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
    VK_CHECK(vkCreateImageView(globals.device.handle, &createInfo, globals.allocator, &image.view.handle));
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

    VK_CHECK(vkAllocateCommandBuffers(globals.device.handle, &allocateInfo, &commandBuffer), __FILE__, __LINE__, "Failed to allocate command buffers");

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo), __FILE__, __LINE__, "Failed to begin command buffer");

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
    VK_CHECK(
        vkQueueSubmit(globals.device.queues.graphics.handle, 1, &submitInfo, VK_NULL_HANDLE),
        __FILE__, __LINE__,
        "Failed to submit command buffers");
    VK_CHECK(
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
    VK_CHECK(
        vkCreateDescriptorPool(globals.device.handle, &descriptorPoolCreateInfo, globals.allocator, &descriptorSets.pool),
        __FILE__, __LINE__,
        "Failed to create descriptor pool");

    auto descriptorSetLayoutCreateInfo = Initializer::descriptorSetLayoutCreateInfo(bindings);
    VK_CHECK(
        vkCreateDescriptorSetLayout(globals.device.handle, &descriptorSetLayoutCreateInfo, globals.allocator, &descriptorSets.setLayout),
        __FILE__, __LINE__,
        "Failed to create descriptor set layout");
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts(globals.swapchain.images.size(), descriptorSets.setLayout);

    descriptorSets.handles.resize(globals.swapchain.images.size());
    auto descriptorSetAllocateInfo = Initializer::descriptorSetAllocateInfo(descriptorSets.pool, globals.swapchain.images.size(), descriptorSetLayouts);
    VK_CHECK(
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
        VK_CHECK(
            vkCreateShaderModule(globals.device.handle, &shaderModuleCreateInfo, globals.allocator, &shaderModule[0]),
            __FILE__, __LINE__,
            "Failed to create shader module");
        stages[0] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, shaderModule[0]);
    }
    {
        auto code = loadShaderCode(pipeline.fsFilename);
        auto shaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(code);
        VK_CHECK(
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
    VK_CHECK(
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
        globals.render_pass);
    VK_CHECK(
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
    VK_CHECK(
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

void load_gltf(Context const& context, std::string const& filename)
{
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string errors;
    std::string warnings;
    if (!loader.LoadASCIIFromFile(&model, &errors, &warnings, filename))
    {
        LOG_ERROR("%s (%u): %s", __FILE__, __LINE__, errors.c_str());
        throw std::runtime_error(errors);
    }

    

    Scene_Manager::get_instance()->scenes.resize(model.scenes.size());
    for (u32 scene_index = 0; scene_index < Scene_Manager::get_instance()->scenes.size(); ++scene_index)
    {
        auto set_node = [=, &model](Scene_Node& node, tinygltf::Node const& tinygltf_node, Scene_Node* parent)
        {
            node.name = tinygltf_node.name;
            node.parent = parent;

            if (!tinygltf_node.matrix.empty())
            {
                node.local_transform = glm::make_mat4(tinygltf_node.matrix.data());
            }
            else
            {
                node.local_transform = glm::translate(glm::mat4(1.0), glm::make_vec3(tinygltf_node.translation.data())) * glm::mat4_cast(glm::make_quat(tinygltf_node.rotation.data())) * glm::scale(glm::mat4(1.0), glm::make_vec3(tinygltf_node.scale.data()));
            }

            if (tinygltf_node.camera != -1 && model.cameras[tinygltf_node.camera].type == "perspective")
            {
                Camera camera;
                camera.name = model.cameras[tinygltf_node.camera].name;
                camera.aspect_ratio = model.cameras[tinygltf_node.camera].perspective.aspectRatio;
                camera.y_fov = model.cameras[tinygltf_node.camera].perspective.yfov;
                camera.z_near = model.cameras[tinygltf_node.camera].perspective.znear;
                camera.z_far = model.cameras[tinygltf_node.camera].perspective.zfar;
                Scene_Manager::get_instance()->scenes[scene_index].cameras.push_back(std::move(camera));
            }

            if (tinygltf_node.mesh != -1)
            {
                auto mesh = std::make_shared<Mesh>();
                mesh->name = model.meshes[tinygltf_node.mesh].name;
                mesh->sub_meshes.resize(model.meshes[tinygltf_node.mesh].primitives.size());
                for (u32 sub_mesh_index = 0; sub_mesh_index < mesh->sub_meshes.size(); ++sub_mesh_index)
                {
                    if (model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].material != -1)
                    {
                        mesh->sub_meshes[sub_mesh_index].material_name = model.materials[model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].material].name;

                        // TODO: add material resource

                        // Add texture
                        auto texture = std::make_shared<Texture>();
                        texture->name = model.images[model.textures[model.materials[model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].material].pbrMetallicRoughness.baseColorTexture.index].source].name;
                        texture->width = model.images[model.textures[model.materials[model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].material].pbrMetallicRoughness.baseColorTexture.index].source].width;
                        texture->height = model.images[model.textures[model.materials[model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].material].pbrMetallicRoughness.baseColorTexture.index].source].height;
                        texture->channel_count = model.images[model.textures[model.materials[model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].material].pbrMetallicRoughness.baseColorTexture.index].source].component;
                        texture->data = model.images[model.textures[model.materials[model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].material].pbrMetallicRoughness.baseColorTexture.index].source].image;
                        Texture_Manager::get_instance()->add_resource(texture->name, texture);
                    }

                    mesh->sub_meshes[sub_mesh_index].first_index = mesh->indices.size();
                    mesh->sub_meshes[sub_mesh_index].vertex_offset = mesh->positions.size();

                    if (model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].attributes.count("POSITION") != 0)
                    {
                        auto& accessor = model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].attributes["POSITION"];
                        auto& buffer_view = model.bufferViews[accessor.bufferView];
                        auto& buffer = model.buffers[buffer_view.buffer];
                        std::vector<glm::vec3> data(accessor.count);
                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                        mesh->positions.insert(mesh->positions.end(), data.begin(), data.end());
                    }

                    if (model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].attributes.count("NORMAL") != 0)
                    {
                        auto& accessor = model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].attributes["NORMAL"];
                        auto& buffer_view = model.bufferViews[accessor.bufferView];
                        auto& buffer = model.buffers[buffer_view.buffer];
                        std::vector<glm::vec3> data(accessor.count);
                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                        mesh->normals.insert(mesh->normals.end(), data.begin(), data.end());
                    }

                    if (model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].attributes.count("TANGENT") != 0)
                    {
                        auto& accessor = model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].attributes["TANGENT"];
                        auto& buffer_view = model.bufferViews[accessor.bufferView];
                        auto& buffer = model.buffers[buffer_view.buffer];
                        std::vector<glm::vec4> data(accessor.count);
                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                        mesh->tangents.insert(mesh->tangents.end(), data.begin(), data.end());
                    }

                    if (model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].attributes.count("TEXCOORD_0") != 0)
                    {
                        auto& accessor = model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].attributes["TEXCOORD_0"];
                        auto& buffer_view = model.bufferViews[accessor.bufferView];
                        auto& buffer = model.buffers[buffer_view.buffer];
                        switch (accessor.componentType)
                        {
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                            {
                                std::vector<glm::u8vec2> data(accessor.count);
                                mesh->tex_coord.reserve(tex_coord.size() + accessor.count);
                                memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                for (u32 i = 0; i < accessor.count; ++i)
                                {
                                    mesh->tex_coord.push_back(glm::vec2(static_cast<float>(data[i][0]) / 255.f, static_cast<float>(data[i][1]) / 255.f));
                                }

                                break;
                            }

                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                            {
                                std::vector<glm::u16vec2> data(accessor.count);
                                mesh->tex_coord.reserve(mesh->tex_coord.size() + accessor.count);
                                memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                for (u32 i = 0; i < accessor.count; ++i)
                                {
                                    mesh->tex_coord.push_back(glm::vec2(static_cast<float>(data[i][0]) / 65535.f, static_cast<float>(data[i][1]) / 65535.f));
                                }

                                break;
                            }

                            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                            {
                                std::vector<glm::vec2> data(accessor.count);
                                memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                mesh->tex_coord.insert(mesh->tex_coord.end(), data.begin(), data.end());

                                break;
                            }

                            default:
                                break;
                        }
                    }

                    if (model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].attributes.count("COLOR_0") != 0)
                    {
                        auto& accessor = model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].attributes["COLOR_0"]];
                        auto& buffer_view = model.bufferViews[accessor.bufferView];
                        auto& buffer = model.buffers[buffer_view.buffer];
                        switch (accessor.type)
                        {
                            case TINYGLTF_TYPE_VEC3:
                            {
                                switch (accessor.componentType)
                                {
                                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                                    {
                                        std::vector<glm::u8vec3> data(accessor.count);
                                        mesh->colors.reserve(mesh->colors.size() + accessor.count);
                                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                        for (u32 i = 0; i < accessor.count; ++i)
                                        {
                                            mesh->colors.push_back(glm::vec4(static_cast<float>(data[i][0]) / 255.f, static_cast<float>(data[i][1]) / 255.f, static_cast<float>(data[i][2]) / 255.f, 1.f));
                                        }

                                        break;
                                    }

                                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                                    {
                                        std::vector<glm::u16vec3> data(accessor.count);
                                        mesh->colors.reserve(mesh->colors.size() + accessor.count);
                                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                        for (u32 i = 0; i < accessor.count; ++i)
                                        {
                                            mesh->colors.push_back(glm::vec4(static_cast<float>(data[i][0]) / 65535.f, static_cast<float>(data[i][1]) / 65535.f, static_cast<float>(data[i][2]) / 65535.f, 1.f));
                                        }

                                        break;
                                    }

                                    case TINYGLTF_COMPONENT_TYPE_FLOAT:
                                    {
                                        std::vector<glm::vec3> data(accessor.count);
                                        mesh->colors.reserve(mesh->colors.size() + accessor.count);
                                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                        for (u32 i = 0; i < accessor.count; ++i)
                                        {
                                            mesh->colors.push_back(glm::vec4(data[i], 1.f));
                                        }

                                        break;
                                    }

                                    default:
                                        break;
                                }

                                break;
                            }

                            case TINYGLTF_TYPE_VEC4:
                            {
                                switch (accessor.componentType)
                                {
                                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                                    {
                                        std::vector<glm::u8vec4> data(accessor.count);
                                        mesh->colors.reserve(mesh->colors.size() + accessor.count);
                                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                        for (u32 i = 0; i < accessor.count; ++i)
                                        {
                                            mesh->colors.push_back(glm::vec4(static_cast<float>(data[i][0]) / 255.f, static_cast<float>(data[i][1]) / 255.f, static_cast<float>(data[i][2]) / 255.f, static_cast<float>(data[i][3]) / 255.f));
                                        }

                                        break;
                                    }

                                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                                    {
                                        std::vector<glm::u16vec4> data(accessor.count);
                                        mesh->colors.reserve(mesh->colors.size() + accessor.count);
                                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                        for (u32 i = 0; i < accessor.count; ++i)
                                        {
                                            mesh->colors.push_back(glm::vec4(static_cast<float>(data[i][0]) / 65535.f, static_cast<float>(data[i][1]) / 65535.f, static_cast<float>(data[i][2]) / 65535.f, static_cast<float>(data[i][3]) / 65535.f));
                                        }

                                        break;
                                    }

                                    case TINYGLTF_COMPONENT_TYPE_FLOAT:
                                    {
                                        std::vector<glm::vec4> data(accessor.count);
                                        memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                        mesh->colors.insert(mesh->colors.end(), data.begin(), data.end());
                                        break;
                                    }

                                    default:
                                        break;
                                }

                                break;
                            }

                            default:
                                break;
                        }
                    }

                    if (model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].attributes.count("JOINTS_0") != 0)
                    {
                        auto& accessor = model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].attributes["JOINTS_0"]];
                        auto& buffer_view = model.bufferViews[accessor.bufferView];
                        auto& buffer = model.buffers[buffer_view.buffer];
                        switch (accessor.componentType)
                        {
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                            {
                                std::vector<glm::u8vec4> data(accessor.count);
                                mesh->joints.reserve(mesh->joints.size() + accessor.count);
                                memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                for (u32 i = 0; i < accessor.count; ++i)
                                {
                                    mesh->joints.push_back(glm::u16vec4(data[i]));
                                }

                                break;
                            }

                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                            {
                                std::vector<glm::u16vec4> data(accessor.count);
                                mesh->joints.insert(mesh->joints.begin(), data.begin(), data.end());
                                memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                break;
                            }

                            default:
                                break;
                        }
                    }

                    if (model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].attributes.count("WEIGHTS_0") != 0)
                    {
                        auto& accessor = model.accessors[model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].attributes["WEIGHTS_0"]];
                        auto& buffer_view = model.bufferViews[accessor.bufferView];
                        auto& buffer = model.buffers[buffer_view.buffer];
                        switch  (accessor.componentType)
                        {
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                            {
                                std::vector<glm::u8vec2> data(accessor.count);
                                mesh->weights.reserve(mesh->weights.size() + accessor.count);
                                memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                for (u32 i = 0; i < accessor.count; ++i)
                                {
                                    mesh->weights.push_back(glm::vec4(static_cast<float>(data[i][0]) / 255.f, static_cast<float>(data[i][1]) / 255.f, static_cast<float>(data[i][2]) / 255.f, static_cast<float>(data[i][3]) / 255.f));
                                }

                                break;
                            }

                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                            {
                                std::vector<glm::u16vec2> data(accessor.count);
                                mesh->weights.reserve(mesh->weights.size() + accessor.count);
                                memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                for (u32 i = 0; i < accessor.count; ++i)
                                {
                                    mesh->weights.push_back(glm::vec4(static_cast<float>(data[i][0]) / 65535.f, static_cast<float>(data[i][1]) / 65535.f, static_cast<float>(data[i][2]) / 65535.f, static_cast<float>(data[i][3]) / 65535.f));
                                }

                                break;
                            }

                            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                            {
                                std::vector<glm::vec4> data(accessor.count);
                                memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                mesh->weights.insert(mesh->weights.begin(), data.begin(), data.end());
                                break;
                            }

                            default:
                                break;
                        }
                    }

                    if (model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].indices != -1)
                    {
                        auto& accessor = model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].indices;
                        auto& buffer_view = model.bufferViews[accessor.bufferView];
                        auto& buffer = model.buffers[buffer_view.buffer];
                        switch (accessor.componentType)
                        {
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                            {
                                std::vector<u8> data(accessor.count);
                                mesh->indices.reserve(mesh->indices.size() + accessor.count);
                                memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                for (u32 i = 0; i < accessor.count; ++i)
                                {
                                    mesh->indices.push_back(data[i]);
                                }

                                break;
                            }

                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                            {
                                std::vector<u16> data(accessor.count);
                                mesh->indices.reserve(mesh->indices.size() + accessor.count);
                                memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                for (u32 i = 0; i < accessor.count; ++i)
                                {
                                    mesh->indices.push_back(data[i]);
                                }

                                break;
                            }

                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                            {
                                std::vector<u32> data(accessor.count);
                                memcpy(data.data(), buffer.data.data() + accessor.byteOffset + buffer_view.byteOffset, buffer_view.byteLength);
                                mesh->indices.insert(indices.begin(), data.begin(), data.end());
                                break;
                            }

                            default:
                                break;
                        }

                        mesh->sub_meshes[sub_mesh_index].index_count = accessor.count;
                    }
                }

                Mesh_Manager::get_instance()->add_resource(mesh->name, mesh);
            }

            node.children.resize(tinygltf_node.children.size());
            for (u32 child_index = 0; child_index < node.children.size(); ++child_index)
            {
                set_node(node.children[child_index], model.nodes[tinygltf_node.children[child_index]], &node);
            }
        }

        Scene_Manager::get_instance()->scenes[scene_index].root_nodes.resize(model.scenes[scene_index].nodes.size());
        for (u32 root_node_index = 0; root_node_index < Scene_Manager::get_instance()->scenes[scene_index].root_nodes.size(); ++root_node_index)
        {
            set_node(Scene_Manager::get_instance()->scenes[scene_index].root_nodes[root_node_index], model.nodes[model.scenes[scene_index].nodes[root_node_index]], nullptr);
        }
    }

}
