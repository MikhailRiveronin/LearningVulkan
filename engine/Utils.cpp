#include "Utils.h"
#include "vulkan_struct_initializers.h"
#include "scene_manager.h"

#include "mesh.h"
#include "mesh_manager.h"
// #include "texture.h"
// #include "texture_manager.h"

#include "structures.h"

#include "third_party/tiny_gltf.h"

#include "third_party/stb_image.h"
#include <fstream>

std::vector<char> load_shader_code(std::string const& filename)
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

void create_buffer(Context const& context, VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocation_create_flags, Buffer& buffer)
{
    auto buffer_create_info = Vulkan_Struct_Initializers::buffer_create_info(size, usage);
    auto allocation_create_info = Vulkan_Struct_Initializers::allocation_create_info(allocation_create_flags);
    VK_CHECK(vmaCreateBuffer(context.allocator, &buffer_create_info, &allocation_create_info, &buffer.handle, &buffer.allocation, &buffer.allocation_info));
}

void create_staging_buffer(Context const& context, VkDeviceSize size, Buffer& buffer)
{
    create_buffer(context, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT, buffer);    
}

void destroy_buffer(Context const& context, Buffer& buffer)
{
    vmaDestroyBuffer(context.allocator, buffer.handle, buffer.allocation);
    buffer.handle = VK_NULL_HANDLE;
    buffer.allocation = VK_NULL_HANDLE;
}

void create_image(Context const& context, VkFormat format, u32 width, u32 height, VkImageUsageFlags usage, VmaAllocationCreateFlags allocation_create_flags, Image& image)
{
    auto image_create_info = Vulkan_Struct_Initializers::image_create_info(format, width, height, usage);
    auto allocation_create_info = Vulkan_Struct_Initializers::allocation_create_info(allocation_create_flags);
    VK_CHECK(vmaCreateImage(context.allocator, &image_create_info, &allocation_create_info, &image.handle, &image.allocation, image.allocation_info));
}

void create_texture(Context const& context, VkFormat format, u32 width, u32 height, void const* data, Texture& texture)
{
    Buffer staging_buffer;
    VkDeviceSize size = width * height * 4;
    create_staging_buffer(context, size, staging_buffer);
    memcpy(buffer.allocation_info.pMappedData, data, size);

    create_image(context, format, width, height, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY, texture.image);
    transition_image_layout(context, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, texture.image.handle);
    copy_buffer_to_image(context, staging_buffer, texture.image);
    destroy_buffer(context, staging_buffer);
    transition_image_layout(context, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texture.image.handle);

    auto image_view_create_info = Vulkan_Struct_Initializers::image_view_create_info(texture.image.handle, format);
    VK_CHECK(vkCreateImageView(context.device->handle, &image_view_create_info, context.allocation_callbacks, &texture.image_view.handle));

    auto sampler_create_info = Vulkan_Struct_Initializers::sampler_create_info();
    VK_CHECK(vkCreateSampler(context.device->handle, &sampler_create_info, context.allocation_callbacks, &texture.sampler.handle));
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



void destroy_image(Context const& context, Image& image)
{
    vmaDestroyImage(context.allocator, image.handle, image.allocation);
}

void destroy_attachment(Context const& context, Attachment& attachment)
{
    vmaDestroyImage(context.allocator, attachment.handle, attachment.allocation);
}

void copyBuffer(Context const& globals, Buffer& srcBuffer, Buffer& dstBuffer)
{
    auto commandBuffer = begin_command_buffer_one_time_submit(globals);

    VkBufferCopy bufferCopy = {};
    bufferCopy.srcOffset = 0;
    bufferCopy.dstOffset = 0;
    bufferCopy.size = srcBuffer.size;

    vkCmdCopyBuffer(commandBuffer, srcBuffer.handle, dstBuffer.handle, 1, &bufferCopy);

    end_command_buffer_one_time_submit(globals, commandBuffer);
}

void copy_buffer_to_image(Context const& context, Buffer const& buffer, Image& image)
{
    auto command_buffer = begin_command_buffer_one_time_submit(context);
    auto buffer_image_copy = Vulkan_Struct_Initializers::buffer_image_copy(image.width, image.height)
    vkCmdCopyBufferToImage(command_buffer, buffer.handle, image.handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy);
    end_command_buffer_one_time_submit(context, command_buffer);
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

void transition_image_layout(Context const& context, VkImageLayout old_layout, VkImageLayout new_layout, Image& image)
{
    VkAccessFlags src_access_mask;
    VkAccessFlags dst_access_mask;
    VkPipelineStageFlags src_stage_mask;
    VkPipelineStageFlags dst_stage_mask;
    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        src_access_mask = 0;
        dst_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
        src_stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        src_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
        dst_access_mask = VK_ACCESS_SHADER_READ_BIT;
        src_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dst_stage_mask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::runtime_error("Unsupported layout transition");
    }

    auto command_buffer = begin_command_buffer_one_time_submit(context);
    auto image_memory_barrier = Vulkan_Struct_Initializers::image_memory_barrier(src_access_mask, dst_access_mask, old_layout, new_layout, image.handle);
    vkCmdPipelineBarrier(command_buffer, src_stage_mask, dst_stage_mask, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);
    end_command_buffer_one_time_submit(context, command_buffer);
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

VkCommandBuffer begin_command_buffer_one_time_submit(Context const& globals)
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

void end_command_buffer_one_time_submit(Context const& globals, VkCommandBuffer commandBuffer)
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

// void createDescriptorSets(Context const& globals, std::vector<DescriptorSetBinding> const& descriptorSetBindings, DescriptorSets& descriptorSets)
// {
//     std::vector<VkDescriptorPoolSize> poolSizes(descriptorSetBindings.size());
//     std::vector<VkDescriptorSetLayoutBinding> bindings(descriptorSetBindings.size());
//     for (u32 i = 0; i < descriptorSetBindings.size(); ++i) {
//         poolSizes[i] = Initializer::descriptorPoolSize(descriptorSetBindings[i].binding.descriptorType, globals.swapchain.images.size());
//         bindings[i] = descriptorSetBindings[i].binding;
//     }
//     auto descriptorPoolCreateInfo = Initializer::descriptorPoolCreateInfo(globals.swapchain.images.size(), poolSizes);
//     VK_CHECK(
//         vkCreateDescriptorPool(globals.device.handle, &descriptorPoolCreateInfo, globals.allocator, &descriptorSets.pool),
//         __FILE__, __LINE__,
//         "Failed to create descriptor pool");

//     auto descriptorSetLayoutCreateInfo = Initializer::descriptorSetLayoutCreateInfo(bindings);
//     VK_CHECK(
//         vkCreateDescriptorSetLayout(globals.device.handle, &descriptorSetLayoutCreateInfo, globals.allocator, &descriptorSets.setLayout),
//         __FILE__, __LINE__,
//         "Failed to create descriptor set layout");
//     std::vector<VkDescriptorSetLayout> descriptorSetLayouts(globals.swapchain.images.size(), descriptorSets.setLayout);

//     descriptorSets.handles.resize(globals.swapchain.images.size());
//     auto descriptorSetAllocateInfo = Initializer::descriptorSetAllocateInfo(descriptorSets.pool, globals.swapchain.images.size(), descriptorSetLayouts);
//     VK_CHECK(
//         vkAllocateDescriptorSets(globals.device.handle, &descriptorSetAllocateInfo, descriptorSets.handles.data()),
//         __FILE__, __LINE__,
//         "Failed to allocate descriptor sets");

//     for (u32 i = 0; i < globals.swapchain.images.size(); ++i) {
//         std::vector<VkWriteDescriptorSet> descriptorWrites(descriptorSetBindings.size());
//         for (u32 j = 0; j < descriptorWrites.size(); ++j) {
//             descriptorWrites[j] = descriptorSetBindings[j].descriptorWrite;
//             descriptorWrites[j].dstSet = descriptorSets.handles[i];
//         }
//         vkUpdateDescriptorSets(globals.device.handle, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
//     }
// }

void destroyDescriptorSets(Context const& globals, DescriptorSets& descriptorSets)
{
    vkDestroyDescriptorSetLayout(globals.device.handle, descriptorSets.setLayout, globals.allocator);
    vkDestroyDescriptorPool(globals.device.handle, descriptorSets.pool, globals.allocator);
}

void create_pipeline(Context const& context,  Pipeline& pipeline)
{
    std::vector<VkPipelineShaderStageCreateInfo> stages(2);
    VkShaderModule shaderModule[2];
    {
        auto code = load_shader_code(pipeline.vsFilename);
        auto shaderModuleCreateInfo = Initializer::shaderModuleCreateInfo(code);
        VK_CHECK(
            vkCreateShaderModule(globals.device.handle, &shaderModuleCreateInfo, globals.allocator, &shaderModule[0]),
            __FILE__, __LINE__,
            "Failed to create shader module");
        stages[0] = Initializer::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, shaderModule[0]);
    }
    {
        auto code = load_shader_code(pipeline.fsFilename);
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

VkPipelineShaderStageCreateInfo create_pipeline_shader_stage(Context const& context, std::string const& filename, VkShaderStageFlagBits stage)
{
    VkShaderModule shader_module;


    std::string shader_folder = "D:/Projects/LearningVulkan/build/Shaders/";
    std::string path = shader_folder + filename;
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file");
    }

    auto file_size = (u32)file.tellg();
    std::vector<char> buffer(file_size);
    file.seekg(0);
    file.read(buffer.data(), file_size);
    file.close();


    auto code = load_shader_code(filename);
    auto shader_module_create_info = Vulkan_Struct_Initializers::shader_module_create_info(buffer);
    VK_CHECK(vkCreateShaderModule(context.device->handle, &shader_module_create_info, context.allocation_callbacks, &shader_module));
    return Vulkan_Struct_Initializers::pipelineShaderStageCreateInfo(stage, shader_module);
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
    loader.SetPreserveimageChannels(false);
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
                        // Texture_Manager::get_instance()->add_resource(texture->name, texture);






                        Texture texture;
                        texture.name = model.images[model.textures[model.materials[model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].material].pbrMetallicRoughness.baseColorTexture.index].source].name;
                        VkImageCreateFlags flags = 0;
                        texture.image.image_type = VK_IMAGE_TYPE_2D;
                        texture.image.format = VK_FORMAT_R8G8B8A8_SRGB;
                        texture.image.width = model.images[model.textures[model.materials[model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].material].pbrMetallicRoughness.baseColorTexture.index].source].width;
                        texture.image.height = model.images[model.textures[model.materials[model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].material].pbrMetallicRoughness.baseColorTexture.index].source].height;
                        texture.image.mip_levels = 1;
                        texture.image.array_layers = 1;
                        texture.image.samples = VK_SAMPLE_COUNT_1_BIT;
                        texture.image.tiling = VK_IMAGE_TILING_OPTIMAL;
                        texture.image.usage = 0;
                        texture.image.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                        texture.image.queue_family_index_count = 0;
                        u32* queue_family_indices = nullptr;
                        VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
                        VkMemoryPropertyFlags memory_properties = 0;
                        VmaAllocation allocation;

                        Buffer staging_buffer;
                        create_staging_buffer(context, texture.image.width * texture.image.height * 4, model.images[model.textures[model.materials[model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].material].pbrMetallicRoughness.baseColorTexture.index].source].image.data(), staging_buffer);

                        create_texture(context, model.images[model.textures[model.materials[model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].material].pbrMetallicRoughness.baseColorTexture.index].source].width, model.images[model.textures[model.materials[model.meshes[tinygltf_node.mesh].primitives[sub_mesh_index].material].pbrMetallicRoughness.baseColorTexture.index].source].height, texture);


                        auto image_create_info = Vulkan_Struct_Initializers::image_create_info(format, extent, usage);

                        VmaAllocationCreateInfo allocation_create_info = {};
                        allocation_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
                        VK_CHECK(vmaCreateImage(creator->device->allocator, &image_create_info, &allocation_create_info, &handle, &allocation, 0));




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
