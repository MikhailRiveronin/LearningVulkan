#include "vulkan_struct_initializers.h"

#include "utils.h"

VkDebugUtilsMessengerCreateInfoEXT Vulkan_Struct_Initializers::debug_utils_messenger_create_info(PFN_vkDebugUtilsMessengerCallbackEXT user_callback)
{
    VkDebugUtilsMessengerCreateInfoEXT create_info;
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    create_info.pfnUserCallback = user_callback;
    create_info.pUserData = nullptr;
    return create_info;
}

VkApplicationInfo Vulkan_Struct_Initializers::application_info()
{
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = nullptr;
    app_info.pApplicationName = "Application";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;
    return app_info;
}

VkInstanceCreateInfo Vulkan_Struct_Initializers::instance_create_info(VkApplicationInfo const* app_info, std::vector<char const*> const& enabled_layer_names, std::vector<char const*> const& enabled_extension_names, void const* next)
{
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pNext = next;
    create_info.pApplicationInfo = app_info;
    create_info.enabledLayerCount = enabled_layer_names.size();
    create_info.ppEnabledLayerNames = enabled_layer_names.data();
    create_info.enabledExtensionCount = enabled_extension_names.size();
    create_info.ppEnabledExtensionNames = enabled_extension_names.data();
    return create_info;
}

VkDeviceQueueCreateInfo Vulkan_Struct_Initializers::device_queue_create_info(u32 queue_family_index)
{
    float queue_priority = 1.f;
    VkDeviceQueueCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.queueFamilyIndex = queue_family_index;
    create_info.queueCount = 1;
    create_info.pQueuePriorities = &queue_priority;
    return create_info;
}

VkWin32SurfaceCreateInfoKHR Vulkan_Struct_Initializers::win32_surface_create_info(HWND h_wnd)
{
    VkWin32SurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.hinstance = GetModuleHandle(nullptr);
    create_info.hwnd = h_wnd;
    return create_info;
}

VkSwapchainCreateInfoKHR Vulkan_Struct_Initializers::swapchain_create_info(VkSurfaceKHR surface, u32 min_image_count, VkFormat image_format, VkColorSpaceKHR image_color_space, VkExtent2D image_extent, std::vector<u32> const& queue_family_indices, VkSurfaceTransformFlagBitsKHR pre_transform, VkPresentModeKHR present_mode)
{
    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.surface = surface;
    create_info.minImageCount = min_image_count;
    create_info.imageFormat = image_format;
    create_info.imageColorSpace = image_color_space;
    create_info.imageExtent = image_extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.imageSharingMode = queue_family_indices.size() == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = create_info.imageSharingMode == VK_SHARING_MODE_CONCURRENT ? queue_family_indices.size() : 0;
    create_info.pQueueFamilyIndices = create_info.imageSharingMode == VK_SHARING_MODE_CONCURRENT ? queue_family_indices.data() : nullptr;
    create_info.preTransform = pre_transform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;
    return create_info;
}

VkBufferCreateInfo Vulkan_Struct_Initializers::buffer_create_info(VkDeviceSize size, VkBufferUsageFlags usage)
{
    VkBufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.size = size;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
    return create_info;
}

VkImageCreateInfo Vulkan_Struct_Initializers::image_create_info(VkFormat format, u32 width, u32 height, VkImageUsageFlags usage)
{
    VkImageCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.imageType = VK_IMAGE_TYPE_2D;
    create_info.format = format;
    create_info.extent.width = width;
    create_info.extent.height = height;
    create_info.extent.depth = 1;
    create_info.mipLevels = 1;
    create_info.arrayLayers = 1;
    create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
    create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    return create_info;
}

VkImageViewCreateInfo Vulkan_Struct_Initializers::image_view_create_info(VkImage image, VkFormat format)
{
    VkImageViewCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.image = image;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = format;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;
    return create_info;
}

VkSamplerCreateInfo Vulkan_Struct_Initializers::sampler_create_info()
{
    VkSamplerCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.magFilter = VK_FILTER_LINEAR;
    create_info.minFilter = VK_FILTER_LINEAR;
    create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    create_info.mipLodBias = 0.f;
    create_info.anisotropyEnable = VK_TRUE;
    create_info.maxAnisotropy = 1.f;
    create_info.compareEnable = VK_FALSE;
    create_info.compareOp = VK_COMPARE_OP_NEVER;
    create_info.minLod = 0.f;
    create_info.maxLod = 0.f;
    create_info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    create_info.unnormalizedCoordinates = VK_FALSE;
    return create_info;
}

VkFramebufferCreateInfo Vulkan_Struct_Initializers::framebuffer_create_info(VkRenderPass render_pass, std::vector<VkImageView> const& attachments, VkExtent2D const& extent)
{
    VkFramebufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.renderPass = render_pass;
    create_info.attachmentCount = attachments.size();
    create_info.pAttachments = attachments.data();
    create_info.width = extent.width;
    create_info.height = extent.height;
    create_info.layers = 1;
    return create_info;
}

VkAttachmentDescription Vulkan_Struct_Initializers::attachment_description(VkFormat format, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkImageLayout initial_layout, VkImageLayout final_layout)
{
    VkAttachmentDescription description = {};
    description.flags = 0;
    description.format = format;
    description.samples = VK_SAMPLE_COUNT_1_BIT;
    description.loadOp = load_op;
    description.storeOp = store_op;
    description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    description.initialLayout = initial_layout;
    description.finalLayout = final_layout;
    return description;
}

VkAttachmentReference Vulkan_Struct_Initializers::attachment_reference(u32 attachment, VkImageLayout layout)
{
    VkAttachmentReference reference = {};
    reference.attachment = attachment;
    reference.layout = layout;
    return reference;
}

VkSubpassDescription Vulkan_Struct_Initializers::subpass_description(std::vector<VkAttachmentReference> const& color_attachments, VkAttachmentReference const* depth_stencil_attachment, std::vector<VkAttachmentReference> const& input_attachments)
{
    VkSubpassDescription description = {};
    description.flags = 0;
    description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    description.inputAttachmentCount = input_attachments.size();
    description.pInputAttachments = input_attachments.data();
    description.colorAttachmentCount = color_attachments.size();
    description.pColorAttachments = color_attachments.data();
    description.pResolveAttachments = nullptr;
    description.pDepthStencilAttachment = nullptr;
    description.preserveAttachmentCount = 0;
    description.pPreserveAttachments = nullptr;
    return description;
}

VkSubpassDependency Vulkan_Struct_Initializers::subpass_dependency(u32 src_subpass, u32 dst_subpass, VkPipelineStageFlags src_stage_mask, VkPipelineStageFlags dst_stage_mask, VkAccessFlags src_access_mask, VkAccessFlags dst_access_mask)
{
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = src_subpass;
    dependency.dstSubpass = dst_subpass;
    dependency.srcStageMask = src_stage_mask;
    dependency.dstStageMask = dst_stage_mask;
    dependency.srcAccessMask = src_access_mask;
    dependency.dstAccessMask = dst_access_mask;
    dependency.dependencyFlags = 0;
    return dependency;
}

VkRenderPassCreateInfo Vulkan_Struct_Initializers::render_pass_create_info(std::vector<VkAttachmentDescription> const& attachments, std::vector<VkSubpassDescription> const& subpasses, std::vector<VkSubpassDependency> const& dependencies)
{
    VkRenderPassCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.attachmentCount = attachments.size();
    create_info.pAttachments = attachments.data();
    create_info.subpassCount = subpasses.size();
    create_info.pSubpasses = subpasses.data();
    create_info.dependencyCount = dependencies.size();
    create_info.pDependencies = dependencies.data();
    return create_info;
}

VkCommandBufferBeginInfo Vulkan_Struct_Initializers::command_buffer_begin_info()
{
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.pNext = nullptr;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = nullptr;
    return begin_info;
}



VkSubmitInfo Vulkan_Struct_Initializers::submit_info(VkSemaphore const* wait_semaphore, VkPipelineStageFlags const* wait_dst_stage_mask, VkCommandBuffer const* command_buffer, VkSemaphore const* signal_semaphore)
{
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = nullptr;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphore;
    submit_info.pWaitDstStageMask = wait_dst_stage_mask;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = command_buffer;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphore;
    return submit_info;
}

VkPresentInfoKHR Vulkan_Struct_Initializers::present_info(VkSemaphore const* wait_semaphore, VkSwapchainKHR const* swapchain, u32 const* image_index)
{
    VkPresentInfoKHR present_info;
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = nullptr;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = wait_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchain;
    present_info.pImageIndices = image_index;
    present_info.pResults = nullptr;
    return present_info;
}

VkImageMemoryBarrier Vulkan_Struct_Initializers::image_memory_barrier(VkAccessFlags src_access_mask, VkAccessFlags dst_access_mask, VkImageLayout old_layout, VkImageLayout new_layout, VkImage image)
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.pNext = nullptr;
    barrier.srcAccessMask = src_access_mask;
    barrier.dstAccessMask = dst_access_mask;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    return barrier;
}

VkBufferImageCopy Vulkan_Struct_Initializers::buffer_image_copy(u32 width, u32 height)
{
    VkBufferImageCopy copy = {};
    copy.bufferOffset = 0;
    copy.bufferRowLength = 0;
    copy.bufferImageHeight = 0;
    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.mipLevel = 0;
    copy.imageSubresource.baseArrayLayer = 0;
    copy.imageSubresource.layerCount = 1;
    copy.imageOffset.x = 0;
    copy.imageOffset.y = 0;
    copy.imageOffset.z = 0;
    copy.imageExtent.width = width;
    copy.imageExtent.height = height;
    copy.imageExtent.depth = 1;
    return copy;
}

VkMemoryAllocateInfo Vulkan_Struct_Initializers::memory_allocate_info(VkDeviceSize allocation_size, u32 memory_type_index)
{
    VkMemoryAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate_info.pNext = nullptr;
    allocate_info.allocationSize = allocation_size;
    allocate_info.memoryTypeIndex = memory_type_index;
    return allocate_info;
}

















VkShaderModuleCreateInfo Vulkan_Struct_Initializers::shader_module_create_info(std::vector<char> const& code)
{
    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<u32*>(code.data());
    return create_info;
}

VkPipelineShaderStageCreateInfo Vulkan_Struct_Initializers::pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule module)
{
    VkPipelineShaderStageCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.stage = stage;
    create_info.module = module;
    create_info.pName = "main";
    create_info.pSpecializationInfo = nullptr;
    return create_info;
}

VkVertexInputBindingDescription Vulkan_Struct_Initializers::vertex_input_binding_description(u32 binding, u32 stride)
{
    VkVertexInputBindingDescription description = {};
    description.binding = binding;
    description.stride = stride;
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return description;
}

VkVertexInputAttributeDescription Vulkan_Struct_Initializers::vertex_input_attribute_description(u32 location, u32 binding, VkFormat format, u32 offset)
{
    VkVertexInputAttributeDescription description = {};
    description.location = location;
    description.binding = binding;
    description.format = format;
    description.offset = offset;
    return description;
}

VkPipelineVertexInputStateCreateInfo Vulkan_Struct_Initializers::pipeline_vertex_input_state_create_info(std::vector<VkVertexInputBindingDescription> const& vertex_binding_descriptions, std::vector<VkVertexInputAttributeDescription> const& vertex_attribute_descriptions)
{
    VkPipelineVertexInputStateCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.vertexBindingDescriptionCount = vertex_binding_descriptions.size();
    create_info.pVertexBindingDescriptions = vertex_binding_descriptions.data();
    create_info.vertexAttributeDescriptionCount = vertex_attribute_descriptions.size();
    create_info.pVertexAttributeDescriptions = vertex_attribute_descriptions.data();
    return create_info;
}

VkPipelineInputAssemblyStateCreateInfo Vulkan_Struct_Initializers::pipeline_input_assembly_state_create_info(VkPrimitiveTopology topology)
{
    VkPipelineInputAssemblyStateCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.topology = topology;
    create_info.primitiveRestartEnable = VK_FALSE;
    return create_info;
}

VkPipelineTessellationStateCreateInfo Vulkan_Struct_Initializers::pipeline_tessellation_state_create_info()
{
    VkPipelineTessellationStateCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.patchControlPoints = 0;
    return create_info;
}

VkViewport Vulkan_Struct_Initializers::viewport(float width, float height)
{
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
    return viewport;
}

VkRect2D Vulkan_Struct_Initializers::scissor(u32 width, u32 height)
{
    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent.width = width;
    scissor.extent.height = height;
    return scissor;
}

VkPipelineViewportStateCreateInfo Vulkan_Struct_Initializers::pipeline_viewport_state_create_info(std::vector<VkViewport> const& viewports, std::vector<VkRect2D> const& scissors)
{
    VkPipelineViewportStateCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.viewportCount = viewports.size();
    create_info.pViewports = viewports.data();
    create_info.scissorCount = scissors.size();
    create_info.pScissors = scissors.data();
    return create_info;
}

VkPipelineRasterizationStateCreateInfo Vulkan_Struct_Initializers::pipeline_rasterization_state_create_info(VkBool32 depth_clamp_enable, VkBool32 rasterizer_discard_enable, VkPolygonMode polygon_mode, VkCullModeFlags cull_mode, VkFrontFace front_face, VkBool32 depth_bias_enable, float depth_bias_constant_factor, float depth_bias_clamp, float depth_bias_slope_factor, float line_width)
{
    VkPipelineRasterizationStateCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.depthClampEnable = depth_clamp_enable;
    create_info.rasterizerDiscardEnable = rasterizer_discard_enable;
    create_info.polygonMode = polygon_mode;
    create_info.cullMode = cull_mode;
    create_info.frontFace = front_face;
    create_info.depthBiasEnable = depth_bias_enable;
    create_info.depthBiasConstantFactor = depth_bias_constant_factor;
    create_info.depthBiasClamp = depth_bias_clamp;
    create_info.depthBiasSlopeFactor = depth_bias_slope_factor;
    create_info.lineWidth = line_width;
    return create_info;
}

VkPipelineMultisampleStateCreateInfo Vulkan_Struct_Initializers::pipeline_multisample_state_create_info(VkSampleCountFlagBits rasterization_samples, VkBool32 sample_shading_enable, float min_sample_shading, VkSampleMask const* sample_mask, VkBool32 alpha_to_coverage_enable, VkBool32 alpha_to_one_enable)
{
    VkPipelineMultisampleStateCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.rasterizationSamples = rasterization_samples;
    create_info.sampleShadingEnable = sample_shading_enable;
    create_info.minSampleShading = min_sample_shading;
    create_info.pSampleMask = sample_mask;
    create_info.alphaToCoverageEnable = alpha_to_coverage_enable;
    create_info.alphaToOneEnable = alpha_to_one_enable;
    return create_info;
}

VkPipelineDepthStencilStateCreateInfo Vulkan_Struct_Initializers::pipeline_depth_stencil_state_create_info(VkBool32 depth_test_enable, VkBool32 depth_write_enable, VkCompareOp depth_compare_op, VkBool32 depth_bounds_test_enable, VkBool32 stencil_test_enable, VkStencilOpState front, VkStencilOpState back)
{
    VkPipelineDepthStencilStateCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.depthTestEnable = depth_test_enable;
    create_info.depthWriteEnable = depth_write_enable;
    create_info.depthCompareOp = depth_compare_op;
    create_info.depthBoundsTestEnable = depth_bounds_test_enable;
    create_info.stencilTestEnable = stencil_test_enable;
    create_info.front = front;
    create_info.back = back;
    create_info.minDepthBounds = 0.f;
    create_info.maxDepthBounds = 1.f;
    return create_info;
}

VkPipelineColorBlendAttachmentState Vulkan_Struct_Initializers::pipeline_color_blend_attachment_state(VkBool32 blend_enable, VkBlendFactor src_color_blend_factor, VkBlendFactor dst_color_blend_factor, VkBlendOp color_blend_op, VkBlendFactor src_alpha_blend_factor, VkBlendFactor dst_alpha_blend_factor, VkBlendOp alpha_blend_op, VkColorComponentFlags color_write_mask)
{
    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.blendEnable = blend_enable;
    color_blend_attachment.srcColorBlendFactor = src_color_blend_factor;
    color_blend_attachment.dstColorBlendFactor = dst_color_blend_factor;
    color_blend_attachment.colorBlendOp = color_blend_op;
    color_blend_attachment.srcAlphaBlendFactor = src_alpha_blend_factor;
    color_blend_attachment.dstAlphaBlendFactor = dst_alpha_blend_factor;
    color_blend_attachment.alphaBlendOp = alpha_blend_op;
    color_blend_attachment.colorWriteMask = color_write_mask;
    return color_blend_attachment;
}

VkPipelineColorBlendStateCreateInfo Vulkan_Struct_Initializers::pipeline_color_blend_state_create_info(std::vector<VkPipelineColorBlendAttachmentState> const& color_blend_attachments)
{
    VkPipelineColorBlendStateCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.logicOpEnable = VK_FALSE;
    create_info.logicOp = VK_LOGIC_OP_CLEAR;
    create_info.attachmentCount = color_blend_attachments.size();
    create_info.pAttachments = color_blend_attachments.data();
    create_info.blendConstants[0] = { 0.f };
    create_info.blendConstants[1] = { 0.f };
    create_info.blendConstants[2] = { 0.f };
    create_info.blendConstants[3] = { 0.f };
    return create_info;
}

VkPipelineDynamicStateCreateInfo Vulkan_Struct_Initializers::pipeline_dynamic_state_create_info(std::vector<VkDynamicState> const& dynamic_states)
{
    VkPipelineDynamicStateCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.dynamicStateCount = dynamic_states.size();
    create_info.pDynamicStates = dynamic_states.data();
    return create_info;
}

VkPipelineLayoutCreateInfo Vulkan_Struct_Initializers::pipeline_layout_create_info(std::vector<VkDescriptorSetLayout> const& set_layouts)
{
    VkPipelineLayoutCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.setLayoutCount = set_layouts.size();
    create_info.pSetLayouts = set_layouts.data();
    create_info.pushConstantRangeCount = 0;
    create_info.pPushConstantRanges = nullptr;
    return create_info;
}

VkGraphicsPipelineCreateInfo Vulkan_Struct_Initializers::graphics_pipeline_create_info(std::vector<VkPipelineShaderStageCreateInfo> const& stages, VkPipelineInputAssemblyStateCreateInfo const* input_assembly_state, VkPipelineTessellationStateCreateInfo const* tessellation_state, VkPipelineViewportStateCreateInfo const* viewport_state, VkPipelineRasterizationStateCreateInfo const* rasterization_state, VkPipelineMultisampleStateCreateInfo const* multisample_state, VkPipelineDepthStencilStateCreateInfo const* depth_stencil_state, VkPipelineColorBlendStateCreateInfo const* color_blend_state, VkPipelineDynamicStateCreateInfo const* dynamic_state, VkPipelineLayout layout, VkRenderPass render_pass, u32 subpass, VkPipeline base_pipeline_handle, i32 base_pipeline_index)
{
    VkGraphicsPipelineCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.stageCount = stages.size();
    create_info.pStages = stages.data();
    create_info.pVertexInputState = nullptr;
    create_info.pInputAssemblyState = input_assembly_state;
    create_info.pTessellationState = tessellation_state;
    create_info.pViewportState = viewport_state;
    create_info.pRasterizationState = rasterization_state;
    create_info.pMultisampleState = multisample_state;
    create_info.pDepthStencilState = depth_stencil_state;
    create_info.pColorBlendState = color_blend_state;
    create_info.pDynamicState = dynamic_state;
    create_info.layout = layout;
    create_info.renderPass = render_pass;
    create_info.subpass = subpass;
    create_info.basePipelineHandle = base_pipeline_handle;
    create_info.basePipelineIndex = base_pipeline_index;
    return create_info;
}











VkDescriptorSetLayoutBinding Vulkan_Struct_Initializers::descriptor_set_layout_binding(u32 binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags)
{
    VkDescriptorSetLayoutBinding layout_binding = {};
    layout_binding.binding = binding;
    layout_binding.descriptorType = descriptor_type;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = stage_flags;
    layout_binding.pImmutableSamplers = nullptr;
    return layout_binding;
}

VkDescriptorPoolSize Vulkan_Struct_Initializers::descriptor_pool_size(VkDescriptorType type, u32 descriptor_count)
{
    VkDescriptorPoolSize pool_size = {};
    pool_size.type = type;
    pool_size.descriptorCount = descriptor_count;
    return pool_size;
}

VkDescriptorPoolCreateInfo Vulkan_Struct_Initializers::descriptor_pool_create_info(u32 max_sets, std::vector<VkDescriptorPoolSize> const& pool_sizes)
{
    VkDescriptorPoolCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.maxSets = max_sets;
    create_info.poolSizeCount = pool_sizes.size();
    create_info.pPoolSizes = pool_sizes.data();
    return create_info;
}

VkDescriptorSetLayoutBindingFlagsCreateInfo Vulkan_Struct_Initializers::descriptorSetLayoutBindingFlagsCreateInfo(std::vector<VkDescriptorBindingFlags> const& bindingFlags)
{
    VkDescriptorSetLayoutBindingFlagsCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.bindingCount = bindingFlags.size();
    create_info.pBindingFlags = bindingFlags.data();
    return create_info;
}

VkDescriptorSetLayoutCreateInfo Vulkan_Struct_Initializers::descriptor_set_layout_create_info(std::vector<VkDescriptorSetLayoutBinding> const& bindings, void const* next)
{
    VkDescriptorSetLayoutCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    create_info.pNext = next;
    create_info.flags = 0;
    create_info.bindingCount = bindings.size();
    create_info.pBindings = bindings.data();
    return create_info;
}

VkDescriptorSetVariableDescriptorCountAllocateInfo Vulkan_Struct_Initializers::descriptorSetVariableDescriptorCountAllocateInfo(std::vector<u32> const& descriptorCounts)
{
    VkDescriptorSetVariableDescriptorCountAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.descriptorSetCount = descriptorCounts.size();
    allocateInfo.pDescriptorCounts = descriptorCounts.data();
    return allocateInfo;
}

VkDescriptorSetAllocateInfo Vulkan_Struct_Initializers::descriptor_set_allocate_info(VkDescriptorPool descriptor_pool, std::vector<VkDescriptorSetLayout> const& set_layouts, void const* next)
{
    VkDescriptorSetAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.pNext = next;
    allocate_info.descriptorPool = descriptor_pool;
    allocate_info.descriptorSetCount = set_layouts.size();
    allocate_info.pSetLayouts = set_layouts.data();
    return allocate_info;
}

VkDescriptorBufferInfo Vulkan_Struct_Initializers::descriptor_buffer_info(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
{
    VkDescriptorBufferInfo buffer_info = {};
    buffer_info.buffer = buffer;
    buffer_info.offset = offset;
    buffer_info.range = range;
    return buffer_info;
}

VkDescriptorImageInfo Vulkan_Struct_Initializers::descriptor_image_info(VkSampler sampler, VkImageView image_view, VkImageLayout image_layout)
{
    VkDescriptorImageInfo image_info = {};
    image_info.sampler = sampler;
    image_info.imageView = image_view;
    image_info.imageLayout = image_layout;
    return image_info;
}

VkWriteDescriptorSet Vulkan_Struct_Initializers::write_descriptor_set(VkDescriptorSet dst_set, u32 dst_binding, std::vector<VkDescriptorBufferInfo> buffer_infos)
{
    VkWriteDescriptorSet descriptor_write = {};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.pNext = nullptr;
    descriptor_write.dstSet = dst_set;
    descriptor_write.dstBinding = dst_binding;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorCount = buffer_infos.size();
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptor_write.pImageInfo = nullptr;
    descriptor_write.pBufferInfo = buffer_infos.data();
    descriptor_write.pTexelBufferView = nullptr;
    return descriptor_write;
}

VkWriteDescriptorSet Vulkan_Struct_Initializers::write_descriptor_set(VkDescriptorSet dst_set, u32 dst_binding, std::vector<VkDescriptorImageInfo> image_infos)
{
    VkWriteDescriptorSet descriptor_write = {};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.pNext = nullptr;
    descriptor_write.dstSet = dst_set;
    descriptor_write.dstBinding = dst_binding;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorCount = image_infos.size();
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_write.pImageInfo = image_infos.data();
    descriptor_write.pBufferInfo = nullptr;
    descriptor_write.pTexelBufferView = nullptr;
    return descriptor_write;
}




























VkRenderPassBeginInfo Vulkan_Struct_Initializers::renderPassBeginInfo(
    VkRenderPass renderPass,
    VkFramebuffer framebuffer,
    VkRect2D renderArea,
    std::vector<VkClearValue> const& clearValues)
{
    VkRenderPassBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.renderPass = renderPass;
    beginInfo.framebuffer = framebuffer;
    beginInfo.renderArea = renderArea;
    beginInfo.clearValueCount = clearValues.size();
    beginInfo.pClearValues = clearValues.data();
    return beginInfo;
}

VmaAllocationCreateInfo Vulkan_Struct_Initializers::allocation_create_info(VmaAllocationCreateFlags flags)
{
    VmaAllocationCreateInfo create_info = {};
    create_info.flags = flags;
    create_info.usage = VMA_MEMORY_USAGE_AUTO;
    create_info.requiredFlags = 0;
    create_info.preferredFlags = 0;
    create_info.memoryTypeBits = 0;
    create_info.pool = VK_NULL_HANDLE;
    create_info.pUserData = nullptr;
    create_info.priority = 1.f;
    return create_info;
}
